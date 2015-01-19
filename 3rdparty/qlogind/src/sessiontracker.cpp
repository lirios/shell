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

#include "sessiontracker.h"

#include "orgfreedesktoplogin1Manager.h"

SessionTracker::SessionTracker(QObject* parent): QObject(parent)
{
    m_managerIface = new OrgFreedesktopLogin1ManagerInterface("org.freedesktop.login1", "/org/freedesktop/login1", QDBusConnection::systemBus());

    connect(m_managerIface, &OrgFreedesktopLogin1ManagerInterface::SessionNew, this, [=](const QString &, const QDBusObjectPath &objectPath) {
        PendingSession *ps = Session::sessionFromPath(objectPath);
        connect(ps, &PendingInterfaceInternal::finished, this, [=](){
            emit sessionAdded(ps->interface());
        });
    });
}


PendingSessions* SessionTracker::listSessions()
{
    PendingSessions *ps = new PendingSessions;

    auto pendingReply = m_managerIface->ListSessions();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingReply);
    connect(watcher, &QDBusPendingCallWatcher::finished, ps, [=]() {
        QDBusPendingReply<SessionInfoList> reply = *watcher;
        watcher->deleteLater();
        QList<PendingSession*> sessionsToLoad;
        foreach(const SessionInfo &sessionInfo, reply.value()) {
            sessionsToLoad << Session::sessionFromPath(sessionInfo.sessionPath);
        }
        ps->setPendingItems(sessionsToLoad);
    });
    return ps;
}


#include "sessiontracker.moc"
