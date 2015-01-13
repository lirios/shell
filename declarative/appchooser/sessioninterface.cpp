/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusPendingCall>
#include <QDebug>

#include "sessioninterface.h"

const QDBusConnection bus = QDBusConnection::sessionBus();

SessionInterface::SessionInterface()
    : m_interface(new QDBusInterface("org.hawaii.session", "/HawaiiSession", "org.hawaii.session", bus))
{
}

SessionInterface::~SessionInterface()
{
    delete m_interface;
}

bool SessionInterface::canLock() const
{
    return true;
}

bool SessionInterface::canStartNewSession() const
{
    return false;
}

bool SessionInterface::canLogOut() const
{
    return m_interface->call("canLogOut").arguments().at(0).toBool();
}

bool SessionInterface::canPowerOff() const
{
    return m_interface->call("canPowerOff").arguments().at(0).toBool();
}

bool SessionInterface::canRestart() const
{
    return m_interface->call("canRestart").arguments().at(0).toBool();
}

bool SessionInterface::canSuspend() const
{
    return m_interface->call("canSuspend").arguments().at(0).toBool();
}

bool SessionInterface::canHibernate() const
{
    return m_interface->call("canHibernate").arguments().at(0).toBool();
}

bool SessionInterface::canHybridSleep() const
{
    return m_interface->call("canHybridSleep").arguments().at(0).toBool();
}

void SessionInterface::lock()
{
    QDBusInterface interface("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver", bus);
    interface.asyncCall("Lock");
}

void SessionInterface::startNewSession()
{
    QDBusInterface interface("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver", bus);
    interface.asyncCall("Lock");
}

void SessionInterface::logOut()
{
    m_interface->asyncCall("logOut");
}

void SessionInterface::powerOff()
{
    m_interface->asyncCall("powerOff");
}

void SessionInterface::restart()
{
    m_interface->asyncCall("restart");
}

void SessionInterface::suspend()
{
    m_interface->asyncCall("suspend");
}

void SessionInterface::hibernate()
{
    m_interface->asyncCall("hibernate");
}

void SessionInterface::hybridSleep()
{
    m_interface->asyncCall("hybridSleep");
}
