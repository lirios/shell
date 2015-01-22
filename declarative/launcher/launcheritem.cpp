/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini
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

#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

#include "applicationaction.h"
#include "applicationinfo.h"
#include "launcheritem.h"

LauncherItem::LauncherItem(const QString &appId, QObject *parent)
    : QObject(parent)
    , m_pinned(false)
    , m_running(true)
    , m_count(0)
    , m_progress(-1)
    , m_info(new ApplicationInfo(appId, this))
{
    connect(m_info, SIGNAL(stateChanged()), this, SIGNAL(runningChanged()));
    connect(m_info, SIGNAL(focusedChanged()), this, SIGNAL(activeChanged()));
}

LauncherItem::LauncherItem(const QString &appId, bool pinned, QObject *parent)
    : QObject(parent)
    , m_pinned(pinned)
    , m_running(false)
    , m_count(0)
    , m_progress(-1)
    , m_info(new ApplicationInfo(appId, this))
{
    connect(m_info, SIGNAL(stateChanged()), this, SIGNAL(runningChanged()));
    connect(m_info, SIGNAL(focusedChanged()), this, SIGNAL(activeChanged()));
}

QString LauncherItem::appId() const
{
    return m_info->appId();
}

QString LauncherItem::name() const
{
    return m_info->name();
}

QString LauncherItem::comment() const
{
    return m_info->comment();
}

QString LauncherItem::iconName() const
{
    return m_info->iconName();
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
    //return m_info->state() != ApplicationInfo::NotRunning;
    return m_running;
}

bool LauncherItem::isActive() const
{
    return m_info->isFocused();
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
