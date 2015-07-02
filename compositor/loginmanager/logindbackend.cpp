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

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusPendingReply>
#include <QtDBus/QDBusUnixFileDescriptor>

#include "logindbackend.h"
#include "logindsessioninfo.h"
#include "sessionmanager.h"

#include <sys/stat.h>
#include <unistd.h>

Q_LOGGING_CATEGORY(LOGIND_BACKEND, "hawaii.loginmanager.logind")

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

LogindBackend *LogindBackend::create(SessionManager *sm, const QDBusConnection &connection)
{
    LogindBackend *backend = new LogindBackend();
    if (!backend)
        return Q_NULLPTR;
    backend->m_sessionManager = sm;

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

void LogindBackend::takeControl()
{
    if (m_sessionPath.isEmpty() || m_sessionControl)
        return;

    QDBusMessage msg = QDBusMessage::createMethodCall(login1Service, m_sessionPath,
                                                      login1SessionInterface,
                                                      QStringLiteral("TakeControl"));
    msg.setArguments(QVariantList() << QVariant(false));

    QDBusPendingReply<void> call = m_interface->connection().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [this](QDBusPendingCallWatcher *self) {
        QDBusPendingReply<void> reply = *self;
        self->deleteLater();

        if (!reply.isValid()) {
            qCWarning(LOGIND_BACKEND) << "Unable to take session control:" << reply.error().message();
            Q_EMIT sessionControlChanged(false);
            return;
        }

        qCDebug(LOGIND_BACKEND) << "Session control acquired";

        m_sessionControl = true;
        Q_EMIT sessionControlChanged(true);

        m_interface->connection().connect(login1Service, m_sessionPath,
                                          login1SessionInterface,
                                          QStringLiteral("PauseDevice"),
                                          this, SLOT(devicePaused(quint32,quint32,QString)));
    });
}

void LogindBackend::releaseControl()
{
    if (m_sessionPath.isEmpty() || !m_sessionControl)
        return;

    QDBusInterface iface(login1Service, m_sessionPath, login1SessionInterface,
                         m_interface->connection());
    iface.asyncCall(QStringLiteral("ReleaseControl"));

    qCDebug(LOGIND_BACKEND) << "Session control released";

    m_sessionControl = false;
    Q_EMIT sessionControlChanged(false);
}

int LogindBackend::takeDevice(const QString &path)
{
    struct stat s;

    if (::stat(path.toLatin1().constData(), &s) < 0) {
        qCWarning(LOGIND_BACKEND) << "Couldn't stat" << path;
        return -1;
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(login1Service, m_sessionPath,
                                                      login1SessionInterface,
                                                      QStringLiteral("TakeDevice"));
    msg.setArguments(QVariantList() << QVariant(major(s.st_rdev)) << QVariant(minor(s.st_rdev)));

    QDBusMessage reply = m_interface->connection().call(msg);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qCWarning(LOGIND_BACKEND,
                  "Couldn't take device \"%s\": %s",
                  qPrintable(path), qPrintable(reply.errorMessage()));
        return -1;
    }

    return ::dup(reply.arguments().first().value<QDBusUnixFileDescriptor>().fileDescriptor());
}

void LogindBackend::releaseDevice(int fd)
{
    struct stat s;

    if (::fstat(fd, &s) < 0) {
        qCWarning(LOGIND_BACKEND) << "Couldn't stat file descriptor" << fd;
        return;
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(login1Service, m_sessionPath,
                                                      login1SessionInterface,
                                                      QStringLiteral("ReleaseDevice"));
    msg.setArguments(QVariantList() << QVariant(major(s.st_rdev)) << QVariant(minor(s.st_rdev)));

    m_interface->connection().asyncCall(msg);
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
    QDBusInterface iface(login1Service, m_sessionPath, login1SessionInterface,
                         m_interface->connection());
    iface.asyncCall(QStringLiteral("Lock"));
}

void LogindBackend::requestUnlockSession()
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
    if (m_inhibitFd > 0 || m_sessionManager->isLocked())
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

        if (m_sessionManager->isLocked())
            return;

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

    // Unset idle hint at startup so that the login manager
    // will report the flag correctly
    setIdle(false);
}

void LogindBackend::devicePaused(quint32 devMajor, quint32 devMinor, const QString &type)
{
    if (QString::compare(type, QStringLiteral("pause"), Qt::CaseInsensitive) == 0) {
        QDBusMessage msg = QDBusMessage::createMethodCall(login1Service, m_sessionPath,
                                                          login1SessionInterface,
                                                          QStringLiteral("PauseDeviceComplete"));
        msg.setArguments(QVariantList() << QVariant(devMajor) << QVariant(devMinor));
        m_interface->connection().asyncCall(msg);
    }
}

#include "moc_logindbackend.cpp"
