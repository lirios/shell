/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  David Edmundson <david@davidedmundson.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef SESSION_H
#define SESSION_H

#include <QDBusConnection>

#include "orgfreedesktoplogin1Session.h"
#include "pendinginterface.h"

#include "qlogind_export.h"

class Session;

typedef PendingInterface<Session> PendingSession;
typedef QSharedPointer<Session> SessionPtr;

class QLOGIND_EXPORT Session : public OrgFreedesktopLogin1SessionInterface
{
    Q_OBJECT
public:
    static PendingSession* sessionFromPath(const QDBusObjectPath &path, const QDBusConnection &connection = QDBusConnection::systemBus());
    static PendingSession* sessionFromName(const QString &id, const QDBusConnection &connection = QDBusConnection::systemBus());
    static PendingSession* sessionFromPid(uint pid, const QDBusConnection &connection = QDBusConnection::systemBus());

protected:
    static SessionPtr createSession(const QDBusObjectPath &path, const QDBusConnection &connection);
    Session(const QDBusObjectPath &path, const QDBusConnection &connection);
};


#endif // SESSION_H
