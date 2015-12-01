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
#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

#include <qt5xdg/xdgautostart.h>
#include <qt5xdg/xdgdesktopfile.h>

#include "cmakedirs.h"
#include "loginmanager/loginmanager.h"
#include "powermanager/powermanager.h"
#include "sessionmanager.h"

#include <sys/types.h>
#include <signal.h>

Q_LOGGING_CATEGORY(SESSION_MANAGER, "hawaii.session.manager")

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
    , m_loginManager(new LoginManager(this, this))
    , m_powerManager(new PowerManager(this))
    , m_idle(false)
    , m_locked(false)
{
    // Lock and unlock the session
    connect(m_loginManager, &LoginManager::sessionLocked, this, [this] {
        setLocked(true);
    });
    connect(m_loginManager, &LoginManager::sessionUnlocked, this, [this] {
        setLocked(false);
    });

    // Logout session before the system goes off
    connect(m_loginManager, &LoginManager::logOutRequested,
            this, &SessionManager::logOut);
}

bool SessionManager::isIdle() const
{
    return m_idle;
}

void SessionManager::setIdle(bool value)
{
    if (m_idle == value)
        return;

    m_idle = value;
    m_loginManager->setIdle(m_idle);
    Q_EMIT idleChanged(value);
}

void SessionManager::idleInhibit()
{
    //Compositor::instance()->incrementIdleInhibit();
}

void SessionManager::idleUninhibit()
{
    //Compositor::instance()->decrementIdleInhibit();
}

bool SessionManager::isLocked() const
{
    return m_locked;
}

void SessionManager::setLocked(bool value)
{
    if (m_locked == value)
        return;

    m_locked = value;
    Q_EMIT lockedChanged(value);
}

bool SessionManager::canLock() const
{
    // Always true, but in the future we might consider blocking
    // this; might come in handy for kiosk systems
    return true;
}

bool SessionManager::canLogOut()
{
    // Always true, but in the future we might consider blocking
    // logout; might come in handy for kiosk systems
    return true;
}

bool SessionManager::canStartNewSession()
{
    // Always false, but in the future we might consider
    // allowing this
    return false;
}

bool SessionManager::canPowerOff()
{
    return m_powerManager->capabilities() & PowerManager::PowerOff;
}

bool SessionManager::canRestart()
{
    return m_powerManager->capabilities() & PowerManager::Restart;
}

bool SessionManager::canSuspend()
{
    return m_powerManager->capabilities() & PowerManager::Suspend;
}

bool SessionManager::canHibernate()
{
    return m_powerManager->capabilities() & PowerManager::Hibernate;
}

bool SessionManager::canHybridSleep()
{
    return m_powerManager->capabilities() & PowerManager::HybridSleep;
}

void SessionManager::lockSession()
{
    m_loginManager->lockSession();
}

void SessionManager::unlockSession()
{
    m_loginManager->unlockSession();
}

void SessionManager::startNewSession()
{
}

void SessionManager::activateSession(int index)
{
    m_loginManager->switchToVt(index);
}

void SessionManager::logOut()
{
    // Close all applications we launched
    //m_launcher->closeApplications();

    // Exit
    QCoreApplication::quit();
}

void SessionManager::powerOff()
{
    m_powerManager->powerOff();
}

void SessionManager::restart()
{
    m_powerManager->restart();
}

void SessionManager::suspend()
{
    m_powerManager->suspend();
}

void SessionManager::hibernate()
{
    m_powerManager->hibernate();
}

void SessionManager::hybridSleep()
{
    m_powerManager->hybridSleep();
}

void SessionManager::autostart()
{
    Q_FOREACH (const XdgDesktopFile &entry, XdgAutoStart::desktopFileList()) {
        if (!entry.isSuitable(true, QStringLiteral("X-Hawaii")))
            continue;

        qCDebug(SESSION_MANAGER) << "Autostart:" << entry.name() << "from" << entry.fileName();
        //m_launcher->launchEntry(const_cast<XdgDesktopFile *>(&entry));
    }
}

#include "moc_sessionmanager.cpp"
