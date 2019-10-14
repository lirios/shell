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
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

#include "authenticator.h"
#include "qmlauthenticator.h"
#include "sessionmanager/sessionmanager.h"

#include <signal.h>
#include <sys/types.h>

#ifdef HAVE_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif

Q_LOGGING_CATEGORY(SESSION_MANAGER, "liri.session.manager")

/*
 * SessionManager
 */

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
    , m_authenticatorThread(new QThread(this))
    , m_authenticator(new Authenticator)
{
    // Unregister D-Bus service when we are exiting
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [] {
#ifdef HAVE_SYSTEMD
        sd_notify(0, "STOPPING=1");
#endif

        QDBusConnection::sessionBus().unregisterService(QStringLiteral("io.liri.Shell"));
    });

#ifdef HAVE_SYSTEMD
    uint64_t interval = 0;
    if (sd_watchdog_enabled(0, &interval) > 0) {
        if (interval > 0) {
            // Start a keep-alive timer every half of the watchdog interval,
            // and convert it from microseconds to milliseconds
            std::chrono::microseconds us(interval / 2);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(us);
            auto *timer = new QTimer(this);
            timer->setInterval(ms);
            connect(timer, &QTimer::timeout, this, [] {
                sd_notify(0, "WATCHDOG=1");
            });
            timer->start();
        }
    }
#endif

    // Emit when the session is locked or unlocked
    QDBusConnection::sessionBus().connect(
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("/io/liri/SessionManager"),
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("Locked"),
                this, SIGNAL(sessionLocked()));
    QDBusConnection::sessionBus().connect(
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("/io/liri/SessionManager"),
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("Unlocked"),
                this, SIGNAL(sessionUnlocked()));

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

bool SessionManager::isIdle() const
{
    return m_idle;
}

void SessionManager::setIdle(bool value)
{
    if (m_idle == value)
        return;

    auto msg = QDBusMessage::createMethodCall(
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("/io/liri/SessionManager"),
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("SetIdle"));
    msg.setArguments(QVariantList() << value);
    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [&](QDBusPendingCallWatcher *self) {
        QDBusPendingReply<> reply = *self;
        if (reply.isError()) {
            qWarning("Failed to toggle idle flag: %s", qPrintable(reply.error().message()));
        } else {
            m_idle = value;
            emit idleChanged(m_idle);
        }

        self->deleteLater();
    });
}

void SessionManager::registerService()
{
    QDBusConnection::sessionBus().registerService(QStringLiteral("io.liri.Shell"));

#ifdef HAVE_SYSTEMD
    sd_notify(0, "READY=1");
#endif
}

void SessionManager::lock()
{
    auto msg = QDBusMessage::createMethodCall(
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("/io/liri/SessionManager"),
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("Lock"));
    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher *self) {
        QDBusPendingReply<> reply = *self;
        if (reply.isError())
            qWarning("Failed to lock session: %s", qPrintable(reply.error().message()));

        self->deleteLater();
    });
}

void SessionManager::unlock()
{
    auto msg = QDBusMessage::createMethodCall(
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("/io/liri/SessionManager"),
                QStringLiteral("io.liri.SessionManager"),
                QStringLiteral("Unlock"));
    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher *self) {
        QDBusPendingReply<> reply = *self;
        if (reply.isError())
            qWarning("Failed to unlock the session: %s", qPrintable(reply.error().message()));

        self->deleteLater();
    });
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
    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher *self) {
        QDBusPendingReply<> reply = *self;
        if (reply.isError())
            qWarning("Failed to launch application: %s", qPrintable(reply.error().message()));

        self->deleteLater();
    });
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
    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher *self) {
        QDBusPendingReply<> reply = *self;
        if (reply.isError())
            qWarning("Failed to launch application from desktop file: %s", qPrintable(reply.error().message()));

        self->deleteLater();
    });
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
    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher *self) {
        QDBusPendingReply<> reply = *self;
        if (reply.isError())
            qWarning("Failed to launch command: %s", qPrintable(reply.error().message()));

        self->deleteLater();
    });
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
    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher *self) {
        QDBusPendingReply<> reply = *self;
        if (reply.isError())
            qWarning("Failed to set environment: %s", qPrintable(reply.error().message()));

        self->deleteLater();
    });
}

void SessionManager::authenticate(const QString &password, const QJSValue &callback)
{
    if (m_authRequested)
        return;

    (void)new QmlAuthenticator(this, callback);
    QMetaObject::invokeMethod(m_authenticator, "authenticate", Q_ARG(QString, password));
    m_authRequested = true;
}
