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

#include <QtNetwork/QLocalSocket>
#include <QtQml/QQmlComponent>

#include <GreenIsland/Compositor>
#include <GreenIsland/Output>

#include "application.h"
#include "messages.h"
#include "session/sessioninterface.h"

using namespace Hawaii;

static QObject *sessionInterfaceProvider(QQmlEngine *, QJSEngine *)
{
    return SessionInterface::instance();
}

Application::Application(const QString &sessionSocket)
    : QObject()
    , HomeApplication()
    , m_sessionSocket(Q_NULLPTR)
{
    // Register QML plugins
    const char *uri = "org.hawaii.session";
    qmlRegisterSingletonType<SessionInterface>(uri, 1, 0, "SessionInterface", sessionInterfaceProvider);

    // Connect to the session manager
    if (!sessionSocket.isEmpty()) {
        m_sessionSocket = new QLocalSocket(this);
        connect(m_sessionSocket, &QLocalSocket::connected,
                this, &Application::connected);
        connect(m_sessionSocket, &QLocalSocket::disconnected,
                this, &Application::disconnected);
        connect(m_sessionSocket, &QLocalSocket::readyRead,
                this, &Application::readyRead);
        connect(m_sessionSocket, SIGNAL(error(QLocalSocket::LocalSocketError)),
                this, SLOT(error()));
        m_sessionSocket->connectToServer(sessionSocket);
    }
}

void Application::compositorLaunched()
{
    connect(Compositor::instance(), &Compositor::idle, this, [this] {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << quint32(CompositorMessages::SetIdle) << true;
        m_sessionSocket->write(data);
        m_sessionSocket->flush();
    });

    connect(Compositor::instance(), &Compositor::wake, this, [this] {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << quint32(CompositorMessages::SetIdle) << false;
        m_sessionSocket->write(data);
        m_sessionSocket->flush();
    });
}

void Application::connected()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << quint32(CompositorMessages::Connected);
    m_sessionSocket->write(data);
    m_sessionSocket->flush();
}

void Application::disconnected()
{
}

void Application::readyRead()
{
    QDataStream input(m_sessionSocket);

    while (input.device()->bytesAvailable()) {
        // Read type
        quint32 type;
        input >> type;

        // Read message
        switch (SessionMessages(type)) {
        case SessionMessages::IdleInhibit:
            Compositor::instance()->incrementIdleInhibit();
            break;
        case SessionMessages::IdleUninhibit:
            Compositor::instance()->decrementIdleInhibit();
            break;
        default:
            break;
        }
    }
}

void Application::error()
{
}

#include "moc_application.cpp"
