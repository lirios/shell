/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QProcess>
#include <QtGui/QGuiApplication>
#include <QtNetwork/QLocalSocket>
#include <QtQml/QQmlComponent>

#include <GreenIsland/Compositor>
#include <GreenIsland/Output>

#include "application.h"
#include "cmakedirs.h"
#include "messages.h"
#include "session/sessioninterface.h"
#include "socketserver.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
#define qCInfo qCDebug
#endif

Q_LOGGING_CATEGORY(COMPOSITOR, "hawaii.compositor")

using namespace Hawaii;

static QObject *sessionInterfaceProvider(QQmlEngine *, QJSEngine *)
{
    return SessionInterface::instance();
}

Application::Application()
    : QObject()
    , HomeApplication()
    , m_socketServer(new SocketServer(this))
    , m_sessionManager(new QProcess(this))
{
    // Start listening on the socket
    const QString seat = QString::fromLatin1(qgetenv("XDG_SEAT"));
    if (!m_socketServer->start(QStringLiteral("hawaii-") + seat)) {
        qCCritical(COMPOSITOR, "Failed to listen to socket: %s",
                   qPrintable(m_socketServer->errorString()));
        QGuiApplication::exit(1);
    }
    connect(m_socketServer, &SocketServer::connected, this, [this] {
        qCDebug(COMPOSITOR) << "Session manager connected";
    });
    connect(m_socketServer, &SocketServer::disconnected, this, [this] {
        qCDebug(COMPOSITOR) << "Session manager disconnected";
    });
    connect(m_socketServer, &SocketServer::idleInhibitRequested, this, [this] {
        Compositor::instance()->incrementIdleInhibit();
    });
    connect(m_socketServer, &SocketServer::idleUninhibitRequested, this, [this] {
        Compositor::instance()->decrementIdleInhibit();
    });

    // Setup the session manager arguments and connect
    m_sessionManager->setProgram(INSTALL_BINDIR "/hawaii-session");
    connect(m_sessionManager, &QProcess::started,
            this, &Application::sessionManagerStarted);
    connect(m_sessionManager, SIGNAL(finished(int)),
            this, SLOT(sessionManagerStopped(int)));

    // Register QML plugins
    const char *uri = "org.hawaii.session";
    qmlRegisterSingletonType<SessionInterface>(uri, 1, 0, "SessionInterface", sessionInterfaceProvider);
}

Application::~Application()
{
    // Stop the session manager
    m_sessionManager->terminate();
    if (!m_sessionManager->waitForFinished())
        m_sessionManager->kill();
}

void Application::compositorLaunched()
{
    // Start the session manager
    qCDebug(COMPOSITOR) << "Running:"
                        << qPrintable(m_sessionManager->program())
                        << qPrintable(m_sessionManager->arguments().join(' '));
    m_sessionManager->start();
}

void Application::sessionManagerStarted()
{
    qCInfo(COMPOSITOR) << "Session manager start successfully";

    // Send SetIdle to the session manager
    m_idleConnection = connect(Compositor::instance(), &Compositor::idle, this, [this] {
        m_socketServer->sendSetIdle(true);
    });
    m_wakeConnection = connect(Compositor::instance(), &Compositor::wake, this, [this] {
        m_socketServer->sendSetIdle(false);
    });
}

void Application::sessionManagerStopped(int exitCode)
{
    qCInfo(COMPOSITOR) << "Session manager stopped with exit code" << exitCode;

    disconnect(m_idleConnection);
    disconnect(m_wakeConnection);
}

#include "moc_application.cpp"
