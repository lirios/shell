/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

#include "authenticator.h"
#include "qmlauthenticator.h"
#include "loginmanager/loginmanager.h"
#include "sessionmanager/sessionmanager.h"
#include "sessionmanager/screensaver/screensaver.h"
#include "screensaver_adaptor.h"

#include <signal.h>
#include <sys/types.h>

Q_LOGGING_CATEGORY(SESSION_MANAGER, "liri.session.manager")

/*
 * SessionManager
 */

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
    , m_authenticatorThread(new QThread(this))
    , m_authRequested(false)
    , m_authenticator(new Authenticator)
    , m_loginManager(new LoginManager(this, this))
    , m_screenSaver(new ScreenSaver(this))
    , m_idle(false)
    , m_locked(false)
{
    // Lock and unlock the session
    connect(m_loginManager, &LoginManager::sessionLocked, this, [this] { setLocked(true); });
    connect(m_loginManager, &LoginManager::sessionUnlocked, this, [this] { setLocked(false); });

    // Logout session before the system goes off
    connect(m_loginManager, &LoginManager::logOutRequested, this, &SessionManager::logOut);

    // Authenticate in a separate thread
    m_authenticator->moveToThread(m_authenticatorThread);
    m_authenticatorThread->start();
}

SessionManager::~SessionManager()
{
    m_authenticatorThread->quit();
    m_authenticatorThread->wait();
    m_authenticator->deleteLater();
}

bool SessionManager::registerWithDBus()
{
    QDBusConnection bus = QDBusConnection::sessionBus();

    new ScreenSaverAdaptor(m_screenSaver);
    if (!bus.registerObject(QStringLiteral("/org/freedesktop/ScreenSaver"), m_screenSaver)) {
        qCWarning(SESSION_MANAGER,
                  "Couldn't register /org/freedesktop/ScreenSaver D-Bus object: %s",
                  qPrintable(bus.lastError().message()));
        return false;
    }

    return true;
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

    if (value)
        Q_EMIT sessionLocked();
    else
        Q_EMIT sessionUnlocked();
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

void SessionManager::registerService()
{
    QDBusConnection::sessionBus().registerService(QStringLiteral("io.liri.Shell"));
}

void SessionManager::launchApplication(const QString &appId)
{
    auto msg = QDBusMessage::createMethodCall(
                QStringLiteral("io.liri.Launcher"),
                QStringLiteral("/io/liri/Launcher"),
                QStringLiteral("io.liri.Launcher"),
                QStringLiteral("LaunchApplication"));
    QVariantList args;
    args.append(appId);
    msg.setArguments(args);
    QDBusConnection::sessionBus().send(msg);
}

void SessionManager::launchDesktopFile(const QString &fileName)
{
    auto msg = QDBusMessage::createMethodCall(
                QStringLiteral("io.liri.Launcher"),
                QStringLiteral("/io/liri/Launcher"),
                QStringLiteral("io.liri.Launcher"),
                QStringLiteral("LaunchDesktopFile"));
    QVariantList args;
    args.append(fileName);
    msg.setArguments(args);
    QDBusConnection::sessionBus().send(msg);
}

void SessionManager::launchCommand(const QString &command)
{
    auto msg = QDBusMessage::createMethodCall(
                QStringLiteral("io.liri.Launcher"),
                QStringLiteral("/io/liri/Launcher"),
                QStringLiteral("io.liri.Launcher"),
                QStringLiteral("LaunchCommand"));
    QVariantList args;
    args.append(command);
    msg.setArguments(args);
    QDBusConnection::sessionBus().send(msg);
}

void SessionManager::setEnvironment(const QString &key, const QString &value)
{
    qputenv(key.toLocal8Bit().constData(), value.toLocal8Bit());

    auto msg = QDBusMessage::createMethodCall(
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("/io/liri/SessionManager"),
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("SetEnvironment"));
    QVariantList args;
    args.append(key);
    args.append(value);
    msg.setArguments(args);
    QDBusConnection::sessionBus().send(msg);
}

void SessionManager::logOut()
{
    // Unregister service
    QDBusConnection::sessionBus().unregisterService(QStringLiteral("io.liri.Shell"));

    // Exit
    auto msg = QDBusMessage::createMethodCall(
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("/io/liri/SessionManager"),
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("Logout"));
    QDBusConnection::sessionBus().send(msg);
}

void SessionManager::lockSession()
{
    m_loginManager->lockSession();
}

void SessionManager::unlockSession(const QString &password, const QJSValue &callback)
{
    if (m_authRequested)
        return;

    (void)new QmlAuthenticator(this, callback);
    QMetaObject::invokeMethod(m_authenticator, "authenticate", Q_ARG(QString, password));
    m_authRequested = true;
}

void SessionManager::startNewSession()
{
    // TODO: Implement
    qWarning("SessionManager::startNewSession() is not implemented");
}

void SessionManager::activateSession(int index)
{
    m_loginManager->switchToVt(index);
}

void SessionManager::requestLogOut()
{
    if (!canLogOut())
        return;
    Q_EMIT logOutRequested();
}

void SessionManager::requestPowerOff()
{
    Q_EMIT powerOffRequested();
}

void SessionManager::requestRestart()
{
    Q_EMIT restartRequested();
}

void SessionManager::requestSuspend()
{
    Q_EMIT suspendRequested();
}

void SessionManager::requestHibernate()
{
    Q_EMIT hibernateRequested();
}

void SessionManager::requestHybridSleep()
{
    Q_EMIT hybridSleepRequested();
}

void SessionManager::cancelShutdownRequest()
{
    Q_EMIT shutdownRequestCanceled();
}
