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

#pragma once

#include <QtCore/QObject>

#include <QtCore/QDateTime>
#include <QtCore/QLoggingCategory>
#include <QtSql/QSqlDatabase>

#include "appusage.h"

Q_DECLARE_LOGGING_CATEGORY(SESSION_MANAGER)

class UsageTracker : public QObject
{
    Q_OBJECT

public:
    UsageTracker(QObject *parent = nullptr);

    void applicationFocused(const QString &appId);

    void applicationLaunched(const QString &appId);

    // TODO: Call this from QML
    void sessionIdleStateChanged(bool isIdle);

    static UsageTracker *instance();

    inline AppUsage *usageForAppId(const QString &appId) { return usageForAppId(appId, false); }

Q_SIGNALS:
    void updated();

private:
    void normalizeScores();

    void incrementUsageForApp(AppUsage *app);
    void incrementUsageForApp(AppUsage *app, const QDateTime &time);

    void openDatabase();
    void migrate();
    void loadUsage();
    void cleanUsage();

    AppUsage *usageForAppId(const QString &appId, bool createIfNew);

    QList<AppUsage *> m_apps;
    AppUsage *m_watchedApp = nullptr;
    QDateTime m_watchStartTime;
    bool m_isIdle = false;

    QSqlDatabase m_db;
};
