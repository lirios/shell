/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * Original based on an algorithm in GNOME Shell, which is licensed under
 * the GPLv2 or higher.
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include "usagetracker.h"

#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

/*
 * USAGE ALGORITHM
 *
 * The ranking algorithm we use is: every time an app score reaches SCORE_MAX,
 * divide all scores by 2. Scores are raised by 1 unit every SAVE_APPS_TIMEOUT
 * seconds. This mechanism allows the list to update relatively fast when
 * a new app is used intensively.
 * To keep the list clean, and avoid being Big Brother, apps that have not been
 * seen for a week and whose score is below SCORE_MIN are removed.
 */

/*
 * Need 7 continuous seconds of focus
 */
#define FOCUS_TIME_MIN_SECONDS 7

/*
 * If after 7 days we haven't seen an app, purge it
 */
#define USAGE_CLEAN_DAYS 7

/*
 * Data is saved to file SHELL_CONFIG_DIR/DATA_FILENAME
 */
#define DATA_FILENAME "application_state"

/*
 * If we transition to idle, only count this many seconds of usage
 */
#define IDLE_TIME_TRANSITION_SECONDS 30

/*
 * How often we save internally app data, in seconds
 */
#define SAVE_APPS_TIMEOUT_SECONDS (5 * 60)

/*
 * With this value, an app goes from bottom to top of the
 * usage list in 50 hours of use
 */
#define SCORE_MAX (3600 * 50 / FOCUS_TIME_MIN_SECONDS)

/*
 * If an app's score in lower than this and the app has not been used in a week,
 * remove it
 */
#define SCORE_MIN (SCORE_MAX >> 3)

Q_LOGGING_CATEGORY(USAGE_TRACKER, "hawaii.usage")

Q_GLOBAL_STATIC(UsageTracker, s_usageTracker)

UsageTracker::UsageTracker(QObject *parent)
    : QObject(parent)
{
    openDatabase();
    migrate();
    loadUsage();
    cleanUsage();
}

void UsageTracker::openDatabase()
{
    QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE", "app_usage");
    m_db.setDatabaseName(dataDir.filePath("app_usage.db"));

    if (!m_db.open()) {
        qCWarning(USAGE_TRACKER, "Unable to open database: %s",
                  qPrintable(m_db.lastError().text()));
        return;
    } else {
        qCDebug(USAGE_TRACKER) << "Database opened:" << m_db.databaseName();
    }
}

void UsageTracker::migrate()
{
    QSqlQuery query("CREATE TABLE IF NOT EXISTS app_usage (id TEXT PRIMARY KEY, last_seen INTEGER, "
                    "score INTEGER)",
                    m_db);
}

/*
 * When an application is focused, update the score of the previous app and begin tracking the
 * current app.
 */
void UsageTracker::applicationFocused(const QString &appId)
{
    if (m_watchedApp != nullptr)
        incrementUsageForApp(m_watchedApp);

    m_watchedApp = appId.isEmpty() ? nullptr : usageForAppId(appId, true);
    m_watchStartTime = QDateTime::currentDateTime();
}

/*
 * An application was seen as soon as it's launched.
 */
void UsageTracker::applicationLaunched(const QString &appId)
{
    AppUsage *app = usageForAppId(appId, true);

    app->lastSeen = QDateTime::currentDateTime();
}

/*
 * Only track the application for a short period of time after the session idles.
 */
void UsageTracker::sessionIdleStateChanged(bool isIdle)
{
    if (isIdle == m_isIdle)
        return;

    m_isIdle = isIdle;

    if (isIdle) {
        QDateTime endTime = QDateTime::currentDateTime().addSecs(IDLE_TIME_TRANSITION_SECONDS);

        incrementUsageForApp(m_watchedApp, endTime);
    } else {
        m_watchStartTime = QDateTime::currentDateTime();
    }
}

/*
 * Once an app reaches the max score, all scores are cut in half so old apps will move to the bottom
 * of the ranking even if they were used a lot a while ago.
 */
void UsageTracker::normalizeScores()
{
    Q_FOREACH (AppUsage *app, m_apps) {
        app->score /= 2;
    }
}

void UsageTracker::incrementUsageForApp(AppUsage *app)
{
    incrementUsageForApp(app, QDateTime::currentDateTime());
}

/*
 * Update the score for the application based on the number of seconds since we started watching the
 * app last.
 */
void UsageTracker::incrementUsageForApp(AppUsage *app, QDateTime time)
{
    app->lastSeen = time;

    int elapsedSeconds = m_watchStartTime.secsTo(time);

    int usageScore = elapsedSeconds / FOCUS_TIME_MIN_SECONDS;

    if (usageScore > 0) {
        app->score += usageScore;

        if (app->score > SCORE_MAX)
            normalizeScores();

        app->save();

        Q_EMIT updated();
    }
}

/*
 * Load application usage from the SQLite database.
 */
void UsageTracker::loadUsage()
{
    Q_ASSERT(m_apps.isEmpty());

    QSqlQuery query("SELECT * from app_usage", m_db);
    QSqlRecord rec = query.record();

    int idColumn = rec.indexOf("id");
    int lastSeenColumn = rec.indexOf("last_seen");
    int scoreColumn = rec.indexOf("score");

    while (query.next()) {
        QDateTime lastSeen;
        lastSeen.setTime_t(query.value(lastSeenColumn).toInt());

        AppUsage *app = new AppUsage(query.value(idColumn).toString(), this);
        app->lastSeen = lastSeen;
        app->score = query.value(scoreColumn).toInt();

        m_apps << app;
    }
}

/*
 * Remove applications that haven't been used in seven days for which the score is below the minimum
 * value.
 */
void UsageTracker::cleanUsage()
{
    QDateTime weekAgo = QDateTime::currentDateTime().addDays(-7);

    Q_FOREACH (AppUsage *app, m_apps) {
        if (app->score < SCORE_MIN && app->lastSeen < weekAgo) {
            m_apps.removeOne(app);
            app->destroy();
            delete app;
        }
    }
}

AppUsage *UsageTracker::usageForAppId(const QString &appId, bool createIfNew)
{
    if (appId.isEmpty())
        return nullptr;

    Q_FOREACH (AppUsage *app, m_apps) {
        if (app->appId == appId)
            return app;
    }

    if (!createIfNew)
        return nullptr;

    AppUsage *app = new AppUsage(appId, this);
    m_apps << app;

    return app;
}

UsageTracker *UsageTracker::instance() { return s_usageTracker; }
