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

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusPendingReply>
#include <QtDBus/QDBusUnixFileDescriptor>

#include "logindbackend.h"
#include "logindsessioninfo.h"

#include <unistd.h>

Q_LOGGING_CATEGORY(LOGIND_BACKEND, "hawaii.session.loginmanager.logind")

const static QString login1Service = QStringLiteral("org.freedesktop.login1");
const static QString login1Object = QStringLiteral("/org/freedesktop/login1");
const static QString login1ManagerInterface = QStringLiteral("org.freedesktop.login1.Manager");
const static QString login1SessionInterface = QStringLiteral("org.freedesktop.login1.Session");
const static QString dbusPropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");

LogindBackend::LogindBackend()
    : LoginManagerBackend()
    , m_inhibitFd(-1)
{
}

LogindBackend::~LogindBackend()
{
    if (m_inhibitFd >= 0)
        ::close(m_inhibitFd);
    delete m_interface;
}

LogindBackend *LogindBackend::create(const QDBusConnection &connection)
{
    LogindBackend *backend = new LogindBackend();
    if (!backend)
        return Q_NULLPTR;

    // Connect to logind if available
    backend->m_interface = new QDBusInterface(login1Service, login1Object,
                                              login1ManagerInterface,
                                              connection);
    if (!backend->m_interface || !backend->m_interface->isValid()) {
        delete backend;
        return Q_NULLPTR;
    }

    // Get notified when the system is preparing for sleep
    backend->m_interface->connection().connect(login1Service, login1Object,
                                               login1ManagerInterface,
                                               QStringLiteral("PrepareForSleep"),
                                               backend, SLOT(prepareForSleep(bool)));

    // Get notified when the system is preparing for shutdown
    backend->m_interface->connection().connect(login1Service, login1Object,
                                               login1ManagerInterface,
                                               QStringLiteral("PrepareForShutdown"),
                                               backend, SLOT(prepareForShutdown(bool)));

    // Get a hold of the session
    QDBusPendingCall call = backend->m_interface->asyncCall(QStringLiteral("GetSessionByPID"),
                                                            (quint32)getpid());
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call);
    backend->connect(watcher, &QDBusPendingCallWatcher::finished,
                     backend, &LogindBackend::getSession);

    // Lock screen when preparing to sleep and logout before shutdown
    backend->setupInhibitors();

    return backend;
}

QString LogindBackend::name() const
{
    return QStringLiteral("logind");
}

void LogindBackend::setIdle(bool value)
{
    QDBusInterface iface(login1Service, m_sessionPath, login1SessionInterface,
                         m_interface->connection());
    iface.asyncCall(QStringLiteral("SetIdleHint"), value);
}

void LogindBackend::lockSession()
{
    QDBusInterface iface(login1Service, m_sessionPath, login1SessionInterface,
                         m_interface->connection());
    iface.asyncCall(QStringLiteral("Lock"));
}

void LogindBackend::unlockSession()
{
    QDBusInterface iface(login1Service, m_sessionPath, login1SessionInterface,
                         m_interface->connection());
    iface.asyncCall(QStringLiteral("Unlock"));
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

void LogindBackend::switchToVt(int index)
{
    // Find a session whose VTNr is equal to index and activate it
    QDBusPendingCall call = m_interface->asyncCall(QStringLiteral("ListSessions"));
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(call);
    connect(w, &QDBusPendingCallWatcher::finished, this, [this, index](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<LogindSessionInfoList> reply = *watcher;
        watcher->deleteLater();
        if (!reply.isValid())
            return;

        Q_FOREACH (const LogindSessionInfo &sessionInfo, reply.value()) {
            QDBusMessage msg = QDBusMessage::createMethodCall(login1Service, sessionInfo.sessionPath.path(),
                                                              dbusPropertiesInterface,
                                                              QStringLiteral("GetAll"));
            msg.setArguments(QList<QVariant>() << login1SessionInterface);

            QDBusPendingCall call = m_interface->connection().asyncCall(msg, 3000);
            QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(call);
            connect(w, &QDBusPendingCallWatcher::finished, this, [this, index, sessionInfo](QDBusPendingCallWatcher *watcher) {
                QDBusPendingReply<QVariantMap> reply = *watcher;
                watcher->deleteLater();
                if (!reply.isValid())
                    return;

                QVariantMap props = reply.value();

                bool ok = false;
                int vtNr = props.value(QStringLiteral("VTNr")).toInt(&ok);
                if (ok && vtNr == index) {
                    qCDebug(LOGIND_BACKEND)
                            << "Switching to session" << sessionInfo.sessionId
                            << "on vt" << vtNr;
                    m_interface->asyncCall(QStringLiteral("ActivateSession"), sessionInfo.sessionId);
                }
            });
        }
    });
}

void LogindBackend::setupInhibitors()
{
    // Inhibit already in action or session already locked
    //if (m_inhibitFd > 0 || SessionManager::instance()->isLocked())
    if (m_inhibitFd > 0)
        return;

    QDBusPendingCall call = m_interface->asyncCall(QStringLiteral("Inhibit"),
                                                   QStringLiteral("shutdown:sleep"),
                                                   QStringLiteral("Hawaii"),
                                                   QStringLiteral("Hawaii needs to logout before shutdown and lock the screen before sleep"),
                                                   QStringLiteral("delay"));
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(call);
    connect(w, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QDBusUnixFileDescriptor> reply = *watcher;
        watcher->deleteLater();

        m_inhibitFd = -1;

        //if (SessionManager::instance()->isLocked())
        //return;

        if (reply.isError()) {
            qCDebug(LOGIND_BACKEND) << "Inhibit error:" << reply.error().message();
            return;
        }

        if (!reply.isValid()) {
            qCDebug(LOGIND_BACKEND) << "Failed to get shutdown and sleep inhibitors";
            return;
        }

        m_inhibitFd = dup(reply.value().fileDescriptor());
    });
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

void LogindBackend::getSession(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    watcher->deleteLater();
    if (!reply.isValid())
        return;

    m_sessionPath = reply.value().path();
    qCDebug(LOGIND_BACKEND) << "Session path:" << m_sessionPath;

    // Emit signals when the session is locked/unlocked by logind
    m_interface->connection().connect(login1Service, m_sessionPath,
                                      login1SessionInterface,
                                      QStringLiteral("Lock"),
                                      this, SIGNAL(sessionLocked()));
    m_interface->connection().connect(login1Service, m_sessionPath,
                                      login1SessionInterface,
                                      QStringLiteral("Unlock"),
                                      this, SIGNAL(sessionUnlocked()));
}

#include "moc_logindbackend.cpp"
