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
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>

#include "applicationaction.h"
#include "applicationinfo.h"
#include "launcheritem.h"

#include <signal.h>

LauncherItem::LauncherItem(const QString &appId, pid_t pid, QObject *parent)
    : QObject(parent)
    , m_pid(pid)
    , m_pinned(false)
    , m_running(true)
    , m_active(true)
    , m_count(0)
    , m_progress(-1)
    , m_info(new ApplicationInfo(appId, this))
{
    connect(m_info, SIGNAL(stateChanged()), this, SIGNAL(runningChanged()));
}

LauncherItem::LauncherItem(const QString &appId, bool pinned, QObject *parent)
    : QObject(parent)
    , m_pid(0)
    , m_pinned(pinned)
    , m_running(false)
    , m_active(false)
    , m_count(0)
    , m_progress(-1)
    , m_info(new ApplicationInfo(appId, this))
{
    connect(m_info, SIGNAL(stateChanged()), this, SIGNAL(runningChanged()));
}

QString LauncherItem::appId() const
{
    return m_info->appId();
}

QString LauncherItem::desktopFileName() const
{
    return m_info->fileName();
}

pid_t LauncherItem::pid() const
{
    return m_pid;
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

bool LauncherItem::isRunning() const
{
    return m_running;
}

bool LauncherItem::isActive() const
{
    return m_active;
}

void LauncherItem::setActive(bool value)
{
    if (m_active == value)
        return;

    m_active = value;
    Q_EMIT activeChanged();
}

int LauncherItem::count() const
{
    return m_count;
}

int LauncherItem::progress() const
{
    return m_progress;
}

bool LauncherItem::launch()
{
    if (isRunning())
        return true;

    const QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface interface("org.hawaii.session", "/HawaiiSession", "org.hawaii.launcher", bus);
    QDBusMessage msg = interface.call("launchDesktopFile", m_info->fileName());
    bool ran = msg.arguments().at(0).toBool();

    if (ran)
        Q_EMIT launched();

    return ran;
}

bool LauncherItem::quit()
{
    if (!isRunning())
        return false;

    if (m_pid == 0)
        return false;

    if (::kill(m_pid, SIGTERM) != 0) {
        if (::kill(m_pid, SIGKILL) != 0)
            return false;
    }

    return true;
}

void LauncherItem::setPid(pid_t pid)
{
    if (m_pid == pid)
        return;

    m_pid = pid;
}

void LauncherItem::setPinned(bool value)
{
    if (m_pinned == value)
        return;

    m_pinned = value;
    Q_EMIT pinnedChanged();
}

void LauncherItem::setRunning(bool value)
{
    if (m_running == value)
        return;

    m_running = value;
    Q_EMIT runningChanged();
}

#include "moc_launcheritem.cpp"
