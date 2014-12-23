/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QDebug>
#include "launcheritem.h"

LauncherItem::LauncherItem(const QString &appId, QObject *parent)
    : QObject(parent)
    , m_appId(appId)
    , m_pinned(false)
    , m_count(0)
    , m_progress(-1)
    , m_service(KService::serviceByStorageId(appId + ".desktop"))
{
    qDebug() << "****" << m_service->name() << m_service->icon();
}

QString LauncherItem::name() const
{
    return m_service->name();
}

QString LauncherItem::iconName() const
{
    return m_service->icon();
}

QString LauncherItem::appId() const
{
    return m_appId;
}

bool LauncherItem::isPinned() const
{
    return m_pinned;
}

void LauncherItem::setPinned(bool value)
{
    if (m_pinned == value)
        return;

    m_pinned = value;
    Q_EMIT pinnedChanged();
}

bool LauncherItem::isRunning() const
{
    return false;
}

bool LauncherItem::isActive() const
{
    return false;
}

int LauncherItem::count() const
{
    return m_count;
}

int LauncherItem::progress() const
{
    return m_progress;
}

#include "moc_launcheritem.cpp"
