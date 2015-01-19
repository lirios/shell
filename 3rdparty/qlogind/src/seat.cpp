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

#include "seat.h"

#include "orgfreedesktoplogin1Manager.h"

#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>


PendingSeat* Seat::seatFromPath(const QDBusObjectPath& path, const QDBusConnection& connection)
{
    PendingSeat *ps = new PendingSeat();
    ps->setInterface(createSeat(path, connection));
    return ps;
}

PendingSeat* Seat::seatFromName(const QString &name, const QDBusConnection& connection)
{
    PendingSeat *ps = new PendingSeat();

    OrgFreedesktopLogin1ManagerInterface manager("org.freedesktop.login1", "/org/freedesktop/login1", connection);
    QDBusPendingReply<QDBusObjectPath> pendingReply = manager.GetSeat(name);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingReply);
    connect(watcher, &QDBusPendingCallWatcher::finished, ps, [=]() {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        watcher->deleteLater();
        ps->setInterface(createSeat(reply.value(), connection));
    });

    return ps;
}

SeatPtr Seat::createSeat(const QDBusObjectPath& path, const QDBusConnection& connection)
{
    static QHash<QDBusObjectPath, QWeakPointer<Seat> > m_sessions;
    if (m_sessions.contains(path)) {
        QWeakPointer <Seat> s = m_sessions[path];
        if (s) {
            return s.toStrongRef();
        }
    }
    SeatPtr s(new Seat(path, connection));
    m_sessions.insert(path, s);

    //if this seat is removed remove it from our list of cached session objects
    //seats re-use IDs and we need new seats created on the same path gets a new object
    OrgFreedesktopLogin1ManagerInterface  *m = new OrgFreedesktopLogin1ManagerInterface("org.freedesktop.login1", "/org/freedesktop/login1", connection, s.data());
    QObject::connect(m, &OrgFreedesktopLogin1ManagerInterface::SeatRemoved, [=](const QString &seatName, const QDBusObjectPath &seatPath) {
        Q_UNUSED(seatName);
        m_sessions.remove(seatPath);
    });

    return s;
}


Seat::Seat(const QDBusObjectPath& path, const QDBusConnection& connection):
    OrgFreedesktopLogin1SeatInterface("org.freedesktop.login1", path.path(), connection)
{
}
