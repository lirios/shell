// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>
#include <QStandardPaths>
#include <QVariant>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "application.h"
#include "usagetracker.h"

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
        dataDir.mkpath(QStringLiteral("."));
    }

    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("app_usage"));
    m_db.setDatabaseName(dataDir.filePath(QStringLiteral("app_usage.db")));

    if (!m_db.open()) {
        qCWarning(lcTaskManager, "Unable to open database: %s",
                  qPrintable(m_db.lastError().text()));
        return;
    } else {
        qCDebug(lcTaskManager) << "Database opened:" << m_db.databaseName();
    }
}

void UsageTracker::migrate()
{
    QSqlQuery query(QStringLiteral("CREATE TABLE IF NOT EXISTS app_usage "
                                   "(id TEXT PRIMARY KEY, last_seen INTEGER, "
                                   "score INTEGER)"),
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
    for (AppUsage *app : qAsConst(m_apps))
        app->score /= 2;
}

void UsageTracker::incrementUsageForApp(AppUsage *app)
{
    incrementUsageForApp(app, QDateTime::currentDateTime());
}

/*
 * Update the score for the application based on the number of seconds since we started watching the
 * app last.
 */
void UsageTracker::incrementUsageForApp(AppUsage *app, const QDateTime &time)
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

    QSqlQuery query(QStringLiteral("SELECT * from app_usage"), m_db);
    QSqlRecord rec = query.record();

    int idColumn = rec.indexOf(QStringLiteral("id"));
    int lastSeenColumn = rec.indexOf(QStringLiteral("last_seen"));
    int scoreColumn = rec.indexOf(QStringLiteral("score"));

    while (query.next()) {
        QDateTime lastSeen;
        lastSeen.setTime_t(query.value(lastSeenColumn).toUInt());

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

    for (AppUsage *app : qAsConst(m_apps)) {
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

    for (AppUsage *app : qAsConst(m_apps)) {
        if (app->appId == appId)
            return app;
    }

    if (!createIfNew)
        return nullptr;

    AppUsage *app = new AppUsage(appId, this);
    m_apps << app;

    return app;
}

UsageTracker *UsageTracker::instance()
{
    return s_usageTracker;
}
