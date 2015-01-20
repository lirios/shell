/****************************************************************************
 * This file is part of Hawaii.
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

#include "sessioninterface.h"

const QDBusConnection bus = QDBusConnection::sessionBus();

SessionInterface::SessionInterface(QObject *parent)
    : QObject(parent)
    , m_interface(new QDBusInterface("org.hawaii.session", "/HawaiiSession", "org.hawaii.session", bus))
    , m_canLock(true)
    , m_canStartNewSession(false)
    , m_canLogOut(false)
    , m_canPowerOff(false)
    , m_canRestart(false)
    , m_canSuspend(false)
    , m_canHibernate(false)
    , m_canHybridSleep(false)
{
    m_canLock = m_interface->call("canLock").arguments().at(0).toBool();
    m_canStartNewSession = m_interface->call("canStartNewSession").arguments().at(0).toBool();
    m_canLogOut = m_interface->call("canLogOut").arguments().at(0).toBool();
    m_canPowerOff = m_interface->call("canPowerOff").arguments().at(0).toBool();
    m_canRestart = m_interface->call("canRestart").arguments().at(0).toBool();
    m_canSuspend = m_interface->call("canSuspend").arguments().at(0).toBool();
    m_canHibernate = m_interface->call("canHibernate").arguments().at(0).toBool();
    m_canHybridSleep = m_interface->call("canHybridSleep").arguments().at(0).toBool();

    QDBusConnection::sessionBus().connect(
                "org.hawaii.session", "/HawaiiSession",
                "org.hawaii.session", "idleChanged",
                this, SIGNAL(idleChanged()));
    QDBusConnection::sessionBus().connect(
                "org.hawaii.session", "/HawaiiSession",
                "org.hawaii.session", "sessionLocked",
                this, SIGNAL(sessionLocked()));
    QDBusConnection::sessionBus().connect(
                "org.hawaii.session", "/HawaiiSession",
                "org.hawaii.session", "sessionUnlocked",
                this, SIGNAL(sessionUnlocked()));
}

SessionInterface::~SessionInterface()
{
    delete m_interface;
}

bool SessionInterface::isIdle() const
{
    return m_interface->property("idle").toBool();
}

void SessionInterface::setIdle(bool value)
{
    m_interface->setProperty("idle", value);
}

bool SessionInterface::canLock() const
{
    return m_canLock;
}

bool SessionInterface::canStartNewSession() const
{
    return m_canStartNewSession;
}

bool SessionInterface::canLogOut() const
{
    return m_canLogOut;
}

bool SessionInterface::canPowerOff() const
{
    return m_canPowerOff;
}

bool SessionInterface::canRestart() const
{
    return m_canRestart;
}

bool SessionInterface::canSuspend() const
{
    return m_canSuspend;
}

bool SessionInterface::canHibernate() const
{
    return m_canHibernate;
}

bool SessionInterface::canHybridSleep() const
{
    return m_canHybridSleep;
}

void SessionInterface::lockSession()
{
    m_interface->asyncCall("lockSession");
}

void SessionInterface::unlockSession()
{
    m_interface->asyncCall("unlockSession");
}

void SessionInterface::startNewSession()
{
    m_interface->asyncCall("startNewSession");
}

void SessionInterface::activateSession(int index)
{
    m_interface->asyncCall("activateSession", index);
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

#include "moc_sessioninterface.cpp"
