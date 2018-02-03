/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include "logindbackend.h"
#include "sessionmanager/sessionmanager.h"

#include <sys/sysmacros.h>
#include <sys/stat.h>
#include <unistd.h>

Q_LOGGING_CATEGORY(LOGIND_BACKEND, "liri.loginmanager.logind")

using namespace Liri;

LogindBackend::LogindBackend()
    : LoginManagerBackend()
{
}

LogindBackend::~LogindBackend()
{
    if (m_powerButtonFd >= 0)
        ::close(m_powerButtonFd);
    if (m_inhibitFd >= 0)
        ::close(m_inhibitFd);
}

LogindBackend *LogindBackend::create(SessionManager *sm)
{
    LogindBackend *backend = new LogindBackend();
    if (!backend)
        return nullptr;
    backend->m_sessionManager = sm;

    // Check if logind is available
    if (!Logind::checkService())
        return nullptr;

    Liri::Logind *logind = Logind::instance();

    // Connect signals
    connect(logind, &Logind::prepareForSleep, backend, &LogindBackend::prepareForSleep);
    connect(logind, &Logind::prepareForShutdown, backend, &LogindBackend::prepareForShutdown);
    connect(logind, &Logind::hasSessionControlChanged, backend, &LogindBackend::sessionControlChanged);
    connect(logind, &Logind::devicePaused, backend, &LogindBackend::devicePaused);
    connect(logind, &Logind::lockSessionRequested, backend, &LogindBackend::sessionLocked);
    connect(logind, &Logind::unlockSessionRequested, backend, &LogindBackend::sessionUnlocked);
    connect(logind, &Logind::connectedChanged, backend, &LogindBackend::handleConnectedChanged);
    connect(logind, &Logind::inhibited, backend, &LogindBackend::handleInhibited);
    connect(logind, &Logind::uninhibited, backend, &LogindBackend::handleUninhibited);

    // Lock screen when preparing to sleep and logout before shutdown
    backend->setupInhibitors();
    Logind::instance()->inhibit(
                QStringLiteral("Liri Shell/PowerButton"),
                QStringLiteral("Liri Shell handles the power button itself"),
                Logind::InhibitPowerKey | Logind::InhibitSuspendKey | Logind::InhibitHibernateKey,
                Logind::Block);

    return backend;
}

QString LogindBackend::name() const
{
    return QStringLiteral("logind");
}

void LogindBackend::setIdle(bool value)
{
    Logind::instance()->setIdleHint(value);
}

void LogindBackend::takeControl()
{
    Logind::instance()->takeControl();
}

void LogindBackend::releaseControl()
{
    Logind::instance()->releaseControl();
}

int LogindBackend::takeDevice(const QString &path)
{
    return Logind::instance()->takeDevice(path);
}

void LogindBackend::releaseDevice(int fd)
{
    Logind::instance()->releaseDevice(fd);
}

void LogindBackend::lockSession()
{
    Q_EMIT sessionLocked();
}

void LogindBackend::unlockSession()
{
    Q_EMIT sessionUnlocked();
}

void LogindBackend::requestLockSession()
{
    Logind::instance()->lockSession();
}

void LogindBackend::requestUnlockSession()
{
    Logind::instance()->unlockSession();
}

void LogindBackend::locked()
{
    if (m_inhibitFd >= 0) {
        ::close(m_inhibitFd);
        m_inhibitFd = -1;
    }
}

void LogindBackend::unlocked()
{
    setupInhibitors();
}

void LogindBackend::switchToVt(quint32 vt)
{
    Logind::instance()->switchTo(vt);
}

void LogindBackend::setupInhibitors()
{
    // Inhibit already in action or session already locked
    if (m_inhibitFd >= 0 || m_sessionManager->isLocked())
        return;

    Logind::instance()->inhibit(
                QStringLiteral("Liri Shell/Sleep"),
                QStringLiteral("Liri Shell needs to logout before shutdown and lock the screen before sleep"),
                Logind::InhibitShutdown | Logind::InhibitSleep,
                Logind::Delay);
}

void LogindBackend::handleConnectedChanged(bool connected)
{
    // Unset idle hint at startup so that the login manager
    // will report the flag correctly
    if (connected)
        setIdle(false);
}

void LogindBackend::handleInhibited(const QString &who, const QString &why, int fd)
{
    Q_UNUSED(why);

    if (who == QStringLiteral("Liri Shell/PowerButton"))
        m_powerButtonFd = fd;
    else if (who == QStringLiteral("Liri Shell/Sleep"))
        m_inhibitFd = fd;
}

void LogindBackend::handleUninhibited(int fd)
{
    if (m_powerButtonFd == fd)
        m_powerButtonFd = -1;
    else if (m_inhibitFd == fd)
        m_inhibitFd = -1;
}

void LogindBackend::prepareForSleep(bool arg)
{
    // Ask to lock the session when the system is
    // going to sleep
    if (arg)
        Q_EMIT sessionLocked();
}

void LogindBackend::prepareForShutdown(bool arg)
{
    // Ask to lock the session when the system is
    // going to sleep
    if (arg)
        Q_EMIT logOutRequested();
}

void LogindBackend::devicePaused(quint32 devMajor, quint32 devMinor, const QString &type)
{
    if (QString::compare(type, QStringLiteral("pause"), Qt::CaseInsensitive) == 0)
        Logind::instance()->pauseDeviceComplete(devMajor, devMinor);
}

#include "moc_logindbackend.cpp"
