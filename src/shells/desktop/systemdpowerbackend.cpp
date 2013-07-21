/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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
    delete m_interface;
}

PowerCapabilities SystemdPowerBackend::capabilities() const
{
    PowerCapabilities caps = PowerCapability::None;

    QDBusReply<QString> reply;

    reply = m_interface->call(QStringLiteral("CanPowerOff"));
    if (reply.isValid() && reply.value() == QStringLiteral("yes"))
        caps |= PowerCapability::PowerOff;

    reply = m_interface->call(QStringLiteral("CanReboot"));
    if (reply.isValid() && reply.value() == QStringLiteral("yes"))
        caps |= PowerCapability::Restart;

    reply = m_interface->call(QStringLiteral("CanSuspend"));
    if (reply.isValid() && reply.value() == QStringLiteral("yes"))
        caps |= PowerCapability::Suspend;

    reply = m_interface->call(QStringLiteral("CanHibernate"));
    if (reply.isValid() && reply.value() == QStringLiteral("yes"))
        caps |= PowerCapability::Hibernate;

    reply = m_interface->call(QStringLiteral("CanHybridSleep"));
    if (reply.isValid() && reply.value() == QStringLiteral("yes"))
        caps |= PowerCapability::HybridSleep;

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
