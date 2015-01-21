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

#include <QtCore/QCoreApplication>

#include "qlogind/src/sessiontracker.h"

#include "sessionadaptor.h"
#include "sessionmanager.h"

SessionAdaptor::SessionAdaptor(SessionManager *sessionManager)
    : QDBusAbstractAdaptor(sessionManager)
    , m_sessionManager(sessionManager)
    , m_powerManager(new PowerManager(this))
    , m_sessionTracker(new SessionTracker(this))
    , m_idle(false)
{
    // When an action is requested we first log out (which means killing
    // the processes and quitting the compositor), when the session is
    // logged out this slot is called and perform the actual action
    connect(m_sessionManager, SIGNAL(loggedOut()),
            this, SLOT(performAction()));

    // Session interface
    PendingSession *ps = Session::sessionFromPid(QCoreApplication::applicationPid());
    connect(ps, &PendingSession::finished, [=] {
        m_session = ps->interface();
        connect(m_session.data(), &OrgFreedesktopLogin1SessionInterface::Lock, [=] {
            Q_EMIT sessionLocked();
        });
        connect(m_session.data(), &OrgFreedesktopLogin1SessionInterface::Unlock, [=] {
            Q_EMIT sessionUnlocked();
        });
    });

    // Manager interface
    PendingManager *pm = Manager::manager();
    connect(pm, &PendingManager::finished, [=] {
        m_manager = pm->interface();
        connect(m_manager.data(), &OrgFreedesktopLogin1ManagerInterface::PrepareForShutdown, [=](bool before) {
            // Logout session before the system goes off
            if (before)
                logOut();
        });
        connect(m_manager.data(), &OrgFreedesktopLogin1ManagerInterface::PrepareForSleep, [=](bool before) {
            // Lock session before the system goes to sleep
            if (before && !m_session.isNull())
                m_session->requestLock();
        });
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

    m_session->SetIdleHint(value);
    m_idle = value;
    Q_EMIT idleChanged();
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
    if (!m_session.isNull())
        m_session->requestLock();
}

void SessionAdaptor::unlockSession()
{
    if (!m_session.isNull())
        m_session->requestUnlock();
}

void SessionAdaptor::startNewSession()
{
}

void SessionAdaptor::activateSession(int index)
{
    PendingSessions *pendingSessions = m_sessionTracker->listSessions();
    for (SessionPtr session: pendingSessions->interfaces()) {
        if (session->property("TTY").toInt() == index) {
            session->Activate();
            break;
        }
    }
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
