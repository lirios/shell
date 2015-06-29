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
#include <QtNetwork/QLocalSocket>

#include "messages.h"
#include "sessionmanager.h"
#include "socketclient.h"

Q_LOGGING_CATEGORY(SOCKET, "hawaii.session.socket")

using namespace Hawaii;

SocketClient::SocketClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QLocalSocket(this))
{
    connect(m_socket, &QLocalSocket::connected,
            this, &SocketClient::connected);
    connect(m_socket, &QLocalSocket::disconnected,
            this, &SocketClient::disconnected);
    connect(m_socket, &QLocalSocket::readyRead,
            this, &SocketClient::readyRead);
}

SocketClient::~SocketClient()
{
    stop();
}

bool SocketClient::start()
{
    const QString seat = QString::fromLatin1(qgetenv("XDG_SEAT"));
    const QString socketName = QStringLiteral("hawaii-session-") + seat;

    m_socket->connectToServer(socketName);
    if (!m_socket->waitForConnected()) {
        qCWarning(SOCKET, "Unable to connect to the compositor: %s",
                  qPrintable(m_socket->errorString()));
        qCWarning(SOCKET, "You may want to use 'starthawaii' to start a Hawaii session.");
        return false;
    }

    connect(m_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(error()));

    return true;
}

bool SocketClient::stop()
{
    if (!m_socket->isOpen())
        return true;

    m_socket->disconnectFromServer();
    if (!m_socket->waitForDisconnected()) {
        qCWarning(SOCKET, "Unable to disconnect from the compositor: %s",
                  qPrintable(m_socket->errorString()));
        return false;
    }

    disconnect(m_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
               this, SLOT(error()));

    return true;
}

void SocketClient::sendIdleInhibit()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << quint32(SessionMessages::IdleInhibit);
    m_socket->write(data);
    m_socket->flush();
}

void SocketClient::sendIdleUninhibit()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << quint32(SessionMessages::IdleUninhibit);
    m_socket->write(data);
    m_socket->flush();
}

void SocketClient::sendLogOut()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << quint32(SessionMessages::LogOut);
    m_socket->write(data);
    m_socket->flush();
}

void SocketClient::connected()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << quint32(SessionMessages::Connected);
    m_socket->write(data);
    m_socket->flush();

    qCDebug(SOCKET) << "Connected to" << m_socket->fullServerName();
}

void SocketClient::disconnected()
{
    qCDebug(SOCKET) << "Disconnect from" << m_socket->fullServerName();
}

void SocketClient::readyRead()
{
    QDataStream input(m_socket);

    while (input.device()->bytesAvailable()) {
        // Read type
        quint32 type;
        input >> type;

        // Read message
        switch (CompositorMessages(type)) {
        case CompositorMessages::SetIdle: {
            bool value;
            input >> value;
            SessionManager::instance()->setIdle(value);
            break;
        }
        default:
            break;
        }
    }
}

void SocketClient::error()
{
    qCWarning(SOCKET) << "Error:" << m_socket->errorString();
}

#include "moc_socketclient.cpp"
