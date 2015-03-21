/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#include "sessionadaptor.h"
#include "sessionmanager.h"

SessionAdaptor::SessionAdaptor(SessionManager *sessionManager)
    : QDBusAbstractAdaptor(sessionManager)
    , m_sessionManager(sessionManager)
    , m_powerManager(new PowerManager(this))
    , m_loginManager(new LoginManager(this))
    , m_idle(false)
{
    // Relay session locked/unlocked signals
    connect(m_loginManager, &LoginManager::sessionLocked, this, [this] {
        if (!m_sessionManager->isLocked()) {
            m_sessionManager->setLocked(true);
            Q_EMIT lockedChanged();
        }

        Q_EMIT sessionLocked();
    });
    connect(m_loginManager, &LoginManager::sessionUnlocked, this, [this] {
        if (m_sessionManager->isLocked()) {
            m_sessionManager->setLocked(false);
            Q_EMIT lockedChanged();
        }

        Q_EMIT sessionUnlocked();
    });

    // Logout session before the system goes off
    connect(m_loginManager, &LoginManager::logOutRequested, [=] {
        logOut();
    });
}

bool SessionAdaptor::isIdle() const
{
    return m_idle;
}

void SessionAdaptor::setIdle(bool value)
{
    if (m_idle == value)
        return;

    m_loginManager->setIdle(value);

    m_idle = value;
    Q_EMIT idleChanged();
}

bool SessionAdaptor::isLocked() const
{
    return m_sessionManager->isLocked();
}

bool SessionAdaptor::canLock()
{
    // Always true, but in the future we might consider blocking
    // this; might come in handy for kiosk systems
    return true;
}

bool SessionAdaptor::canLogOut()
{
    // Always true, but in the future we might consider blocking
    // logout; might come in handy for kiosk systems
    return true;
}

bool SessionAdaptor::canStartNewSession()
{
    // Always false, but in the future we might consider
    // allowing this
    return false;
}

bool SessionAdaptor::canPowerOff()
{
    return m_powerManager->capabilities() & PowerManager::PowerOff;
}

bool SessionAdaptor::canRestart()
{
    return m_powerManager->capabilities() & PowerManager::Restart;
}

bool SessionAdaptor::canSuspend()
{
    return m_powerManager->capabilities() & PowerManager::Suspend;
}

bool SessionAdaptor::canHibernate()
{
    return m_powerManager->capabilities() & PowerManager::Hibernate;
}

bool SessionAdaptor::canHybridSleep()
{
    return m_powerManager->capabilities() & PowerManager::HybridSleep;
}

void SessionAdaptor::lockSession()
{
    m_loginManager->lockSession();
}

void SessionAdaptor::unlockSession()
{
    m_loginManager->unlockSession();
}

void SessionAdaptor::startNewSession()
{
}

void SessionAdaptor::activateSession(int index)
{
    m_loginManager->switchToVt(index);
}

void SessionAdaptor::logOut()
{
    m_sessionManager->logOut();
    ::exit(EXIT_SUCCESS);
}

void SessionAdaptor::powerOff()
{
    m_powerManager->powerOff();
}

void SessionAdaptor::restart()
{
    m_powerManager->restart();
}

void SessionAdaptor::suspend()
{
    m_powerManager->suspend();
}

void SessionAdaptor::hibernate()
{
    m_powerManager->hibernate();
}

void SessionAdaptor::hybridSleep()
{
    m_powerManager->hybridSleep();
}

#include "moc_sessionadaptor.cpp"
