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

#include "seattracker.h"

#include "orgfreedesktoplogin1Manager.h"

SeatTracker::SeatTracker(QObject* parent): QObject(parent)
{
    m_managerIface = new OrgFreedesktopLogin1ManagerInterface("org.freedesktop.login1", "/org/freedesktop/login1", QDBusConnection::systemBus());

    connect(m_managerIface, &OrgFreedesktopLogin1ManagerInterface::SeatNew, this, [=](const QString &, const QDBusObjectPath &objectPath) {
        PendingSeat *ps = Seat::seatFromPath(objectPath);
        connect(ps, &PendingInterfaceInternal::finished, this, [=](){
            emit seatAdded(ps->interface());
        });
    });
}

PendingSeats* SeatTracker::listSeats()
{
    PendingSeats *ps = new PendingSeats;

    auto pendingReply = m_managerIface->ListSeats();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingReply);
    connect(watcher, &QDBusPendingCallWatcher::finished, ps, [=]() {
        QDBusPendingReply<NamedSeatPathList> reply = *watcher;
        watcher->deleteLater();
        QList<PendingSeat*> seatsToLoad;
        foreach(const NamedSeatPath &seat, reply.value()) {
            seatsToLoad << Seat::seatFromPath(seat.path);
        }
        ps->setPendingItems(seatsToLoad);
    });
    return ps;
}


#include "seattracker.moc"
