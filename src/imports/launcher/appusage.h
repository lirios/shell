// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QObject>

#include <QtCore/QDateTime>
#include <QtSql/QSqlDatabase>

class AppUsage : QObject
{
    Q_OBJECT

public:
    AppUsage(const QString &appId, QObject *parent = nullptr);

    QString appId;
    QDateTime lastSeen;
    int score = 0;

    void save() const;
    void destroy() const;

private:
    QSqlDatabase m_db;
};
