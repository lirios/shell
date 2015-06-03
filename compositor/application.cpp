/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
