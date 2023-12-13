// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QVariant>
#include <QSqlQuery>

#include "appusage.h"

AppUsage::AppUsage(const QString &appId, QObject *parent)
    : QObject(parent)
    , appId(appId)
{
    m_db = QSqlDatabase::database(QStringLiteral("app_usage"));
}

void AppUsage::save() const
{
    if (!m_db.isOpen())
        return;

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("INSERT OR REPLACE INTO app_usage VALUES (:id, :last_seen, :score)"));
    query.bindValue(QStringLiteral(":id"), appId);
    query.bindValue(QStringLiteral(":last_seen"), lastSeen.toSecsSinceEpoch());
    query.bindValue(QStringLiteral(":score"), score);
    query.exec();
}

void AppUsage::destroy() const
{
    if (!m_db.isOpen())
        return;

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("DELETE FROM app_usage WHERE app_id = :id"));
    query.bindValue(QStringLiteral(":id"), appId);
    query.exec();
}
