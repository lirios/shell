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

#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtGui/QGuiApplication>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#include "messages.h"
#include "socketserver.h"

Q_LOGGING_CATEGORY(SOCKETSERVER, "hawaii.compositor.socket")

using namespace Hawaii;

SocketServer::SocketServer(QObject *parent)
    : QObject(parent)
    , m_server(Q_NULLPTR)
    , m_client(Q_NULLPTR)
{
}

SocketServer::~SocketServer()
{
    stop();
}

QString SocketServer::address() const
{
    if (m_server)
        return m_server->fullServerName();
    return QString();
}

QString SocketServer::errorString() const
{
    if (m_server)
        return m_server->errorString();
    return QString();
}

bool SocketServer::start(const QString &socketName)
{
    // Do not start twice
    if (m_server)
        return true;

    m_socketName = socketName;

    m_server = new QLocalServer(this);
    m_server->setSocketOptions(QLocalServer::UserAccessOption);
    m_server->removeServer(socketName);
    if (m_server->listen(socketName)) {
        qCDebug(SOCKETSERVER) << "Listening on" << socketName;
        connect(m_server, &QLocalServer::newConnection,
                this, &SocketServer::newConnection);
    } else {
        qCWarning(SOCKETSERVER) << "Failed to listen on" << socketName;
        return false;
    }

    return true;
}

void SocketServer::stop()
{
    if (!m_server)
        return;

    qCDebug(SOCKETSERVER) << "Stopping server...";

    m_server->close();

    m_server->deleteLater();
    m_server = Q_NULLPTR;

    if (!m_socketName.isEmpty()) {
        qCDebug(SOCKETSERVER) << "Remove" << m_socketName;
        QFile::remove(m_socketName);
    }

    m_socketName = QString();

    qCDebug(SOCKETSERVER) << "Server stopped";
}

void SocketServer::sendSetIdle(bool flag)
{
    if (!m_client)
        return;

    qCDebug(SOCKETSERVER) << "Send SetIdle" << flag;

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << quint32(CompositorMessages::SetIdle) << flag;
    m_client->write(data);
}

void SocketServer::newConnection()
{
    if (m_client)
        return;

    m_client = m_server->nextPendingConnection();
    connect(m_client, &QLocalSocket::readyRead,
            this, &SocketServer::readyRead);
    connect(m_client, &QLocalSocket::disconnected, this, [this] {
        Q_EMIT disconnected();
        m_client->deleteLater();
        m_client = Q_NULLPTR;
    });
}

void SocketServer::readyRead()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());
    if (!socket)
        return;

    QDataStream input(socket);

    // Read message type
    quint32 type;
    input >> type;

    // Read messages
    switch (SessionMessages(type)) {
    case SessionMessages::Connected:
        qCDebug(SOCKETSERVER) << "Session manager connection established";
        Q_EMIT connected();
        break;
    case SessionMessages::IdleInhibit:
        Q_EMIT idleInhibitRequested();
        break;
    case SessionMessages::IdleUninhibit:
        Q_EMIT idleUninhibitRequested();
        break;
    case SessionMessages::LogOut:
        QGuiApplication::quit();
        break;
    default:
        break;
    }
}

#include "moc_socketserver.cpp"
