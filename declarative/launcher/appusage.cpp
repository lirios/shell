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

#include "appusage.h"

#include <QtCore/QVariant>
#include <QtSql/QSqlQuery>

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
    query.bindValue(QStringLiteral(":last_seen"), lastSeen.toTime_t());
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
