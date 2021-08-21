// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>

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

void SessionManager::registerService()
{
    if (!QDBusConnection::sessionBus().registerService(QStringLiteral("io.liri.Shell"))) {
        qWarning("Failed to register D-Bus service io.liri.Shell");
        return;
    }

#ifdef HAVE_SYSTEMD
    sd_notify(0, "READY=1");
#endif
}

void SessionManager::authenticate(const QString &password, const QJSValue &callback)
{
    if (m_authRequested)
        return;

    (void)new QmlAuthenticator(this, callback);
    QMetaObject::invokeMethod(m_authenticator, "authenticate", Q_ARG(QString, password));
    m_authRequested = true;
}
