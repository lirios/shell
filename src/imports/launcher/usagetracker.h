// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
