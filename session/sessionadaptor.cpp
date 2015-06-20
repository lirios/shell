/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "sessionadaptor.h"
#include "sessionmanager.h"

SessionAdaptor::SessionAdaptor(SessionManager *sessionManager)
    : QDBusAbstractAdaptor(sessionManager)
    , m_sessionManager(sessionManager)
    , m_powerManager(new PowerManager)
    , m_loginManager(new LoginManager)
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
    connect(m_loginManager, &LoginManager::logOutRequested,
            this, &SessionAdaptor::logOut);

    // Set idle
    connect(m_sessionManager, &SessionManager::idleChanged, this, [this](bool value) {
        m_loginManager->setIdle(value);
    });
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
