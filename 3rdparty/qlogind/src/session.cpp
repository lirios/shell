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

#include "session.h"

#include "orgfreedesktoplogin1Manager.h"

#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>


PendingSession* Session::sessionFromPath(const QDBusObjectPath& path, const QDBusConnection& connection)
{
    PendingSession *ps = new PendingSession();
    ps->setInterface(createSession(path, connection));
    return ps;
}

PendingSession* Session::sessionFromPid(uint pid, const QDBusConnection& connection)
{
    PendingSession *ps = new PendingSession();

    OrgFreedesktopLogin1ManagerInterface manager("org.freedesktop.login1", "/org/freedesktop/login1", connection);
    QDBusPendingReply<QDBusObjectPath> pendingReply = manager.GetSessionByPID(pid);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingReply);
    connect(watcher, &QDBusPendingCallWatcher::finished, ps, [=]() {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        watcher->deleteLater();
        ps->setInterface(createSession(reply.value(), connection));
    });

    return ps;
}

PendingSession* Session::sessionFromName(const QString &name, const QDBusConnection& connection)
{
    PendingSession *ps = new PendingSession();

    OrgFreedesktopLogin1ManagerInterface manager("org.freedesktop.login1", "/org/freedesktop/login1", connection);
    QDBusPendingReply<QDBusObjectPath> pendingReply = manager.GetSession(name);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingReply);
    connect(watcher, &QDBusPendingCallWatcher::finished, ps, [=]() {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        watcher->deleteLater();
        ps->setInterface(createSession(reply.value(), connection));
    });

    return ps;
}

SessionPtr Session::createSession(const QDBusObjectPath& path, const QDBusConnection& connection)
{
    static QHash<QDBusObjectPath, QWeakPointer<Session> > m_sessions;
    if (m_sessions.contains(path)) {
        QWeakPointer <Session> s = m_sessions[path];
        if (s) {
            return s.toStrongRef();
        }
    }
    SessionPtr s(new Session(path, connection));
    m_sessions.insert(path, s);
    return s;
}


Session::Session(const QDBusObjectPath& path, const QDBusConnection& connection):
    OrgFreedesktopLogin1SessionInterface("org.freedesktop.login1", path.path(), connection)
{

}
