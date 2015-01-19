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

#include "usertracker.h"

#include "orgfreedesktoplogin1Manager.h"

UserTracker::UserTracker(QObject* parent): QObject(parent)
{
    m_managerIface = new OrgFreedesktopLogin1ManagerInterface("org.freedesktop.login1", "/org/freedesktop/login1", QDBusConnection::systemBus());
    connect(m_managerIface, &OrgFreedesktopLogin1ManagerInterface::UserNew, this, [=](uint, const QDBusObjectPath &objectPath) {
        PendingUser *ps = User::userFromPath(objectPath);
        connect(ps, &PendingInterfaceInternal::finished, this, [=](){
            emit userAdded(ps->interface());
        });
    });
}

PendingUsers* UserTracker::listUsers()
{
    PendingUsers *pu = new PendingUsers;

    auto pendingReply = m_managerIface->ListUsers();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingReply);
    connect(watcher, &QDBusPendingCallWatcher::finished, pu, [=]() {
        QDBusPendingReply<UserInfoList> reply = *watcher;
        watcher->deleteLater();
        QList<PendingUser*> usersToLoad;
        foreach(const UserInfo &user, reply.value()) {
            usersToLoad << User::userFromPath(user.path);
        }
        pu->setPendingItems(usersToLoad);
    });
    return pu;
}


#include "usertracker.moc"
