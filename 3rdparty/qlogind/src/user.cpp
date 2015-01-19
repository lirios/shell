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

#include "user.h"

#include "orgfreedesktoplogin1Manager.h"

#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>


PendingUser* User::userFromPath(const QDBusObjectPath& path, const QDBusConnection& connection)
{
    PendingUser *ps = new PendingUser();
    ps->setInterface(createUser(path, connection));
    return ps;
}

PendingUser* User::userFromPid(uint pid, const QDBusConnection& connection)
{
    PendingUser *ps = new PendingUser();

    OrgFreedesktopLogin1ManagerInterface manager("org.freedesktop.login1", "/org/freedesktop/login1", connection);
    QDBusPendingReply<QDBusObjectPath> pendingReply = manager.GetUserByPID(pid);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingReply);
    connect(watcher, &QDBusPendingCallWatcher::finished, ps, [=]() {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        watcher->deleteLater();
        ps->setInterface(createUser(reply.value(), connection));
    });

    return ps;
}

PendingUser* User::userFromUid(uint userId, const QDBusConnection& connection)
{
    PendingUser *ps = new PendingUser();

    OrgFreedesktopLogin1ManagerInterface manager("org.freedesktop.login1", "/org/freedesktop/login1", connection);
    QDBusPendingReply<QDBusObjectPath> pendingReply = manager.GetUser(userId);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingReply);
    connect(watcher, &QDBusPendingCallWatcher::finished, ps, [=]() {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        watcher->deleteLater();
        ps->setInterface(createUser(reply.value(), connection));
    });

    return ps;
}

UserPtr User::createUser(const QDBusObjectPath& path, const QDBusConnection& connection)
{
    static QHash<QDBusObjectPath, QWeakPointer<User> > m_users;
    if (m_users.contains(path)) {
        QWeakPointer <User> s = m_users[path];
        if (s) {
            return s.toStrongRef();
        }
    }
    UserPtr s(new User(path, connection));
    m_users.insert(path, s);
    return s;
}


User::User(const QDBusObjectPath& path, const QDBusConnection& connection):
    OrgFreedesktopLogin1UserInterface("org.freedesktop.login1", path.path(), connection)
{

}

#include "user.moc"
