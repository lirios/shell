/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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

#include <QtDBus/QDBusReply>

#include "systemdpowerbackend.h"

#define LOGIN1_SERVICE QStringLiteral("org.freedesktop.login1")
#define LOGIN1_PATH QStringLiteral("/org/freedesktop/login1")
#define LOGIN1_OBJECT QStringLiteral("org.freedesktop.login1.Manager")

QString SystemdPowerBackend::service()
{
    return LOGIN1_SERVICE;
}

SystemdPowerBackend::SystemdPowerBackend()
{
    m_interface = new QDBusInterface(LOGIN1_SERVICE, LOGIN1_PATH,
                                     LOGIN1_OBJECT, QDBusConnection::systemBus());
}

SystemdPowerBackend::~SystemdPowerBackend()
{
    m_interface->deleteLater();
}

QString SystemdPowerBackend::name() const
{
    return QStringLiteral("systemd");
}

PowerManager::Capabilities SystemdPowerBackend::capabilities() const
{
    PowerManager::Capabilities caps = PowerManager::None;

    QStringList validValues;
    validValues << QStringLiteral("yes") << QStringLiteral("challenge");

    QDBusReply<QString> reply;

    reply = m_interface->call(QStringLiteral("CanPowerOff"));
    if (reply.isValid() && validValues.contains(reply.value()))
        caps |= PowerManager::PowerOff;

    reply = m_interface->call(QStringLiteral("CanReboot"));
    if (reply.isValid() && validValues.contains(reply.value()))
        caps |= PowerManager::Restart;

    reply = m_interface->call(QStringLiteral("CanSuspend"));
    if (reply.isValid() && validValues.contains(reply.value()))
        caps |= PowerManager::Suspend;

    reply = m_interface->call(QStringLiteral("CanHibernate"));
    if (reply.isValid() && validValues.contains(reply.value()))
        caps |= PowerManager::Hibernate;

    reply = m_interface->call(QStringLiteral("CanHybridSleep"));
    if (reply.isValid() && validValues.contains(reply.value()))
        caps |= PowerManager::HybridSleep;

    return caps;
}

void SystemdPowerBackend::powerOff()
{
    m_interface->call(QStringLiteral("PowerOff"), true);
}

void SystemdPowerBackend::restart()
{
    m_interface->call(QStringLiteral("Reboot"), true);
}

void SystemdPowerBackend::suspend()
{
    m_interface->call(QStringLiteral("Suspend"), true);
}

void SystemdPowerBackend::hibernate()
{
    m_interface->call(QStringLiteral("Hibernate"), true);
}

void SystemdPowerBackend::hybridSleep()
{
    m_interface->call(QStringLiteral("HybridSleep"), true);
}
