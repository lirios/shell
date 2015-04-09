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

#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>

class QLocalServer;
class QLocalSocket;

Q_DECLARE_LOGGING_CATEGORY(SERVERSOCKET)

class SocketServer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SocketServer)
public:
    SocketServer(QObject *parent = 0);
    ~SocketServer();

    QString address() const;

    bool start(const QString &socketName);
    void stop();

    void sendLock();
    void sendUnlock();

Q_SIGNALS:
    void connected();
    void idleChanged(bool idle);

private:
    QLocalServer *m_server;
    QLocalSocket *m_client;

private Q_SLOTS:
    void newConnection();
    void readyRead();
};

#endif // SOCKETSERVER_H
