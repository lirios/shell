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

#include "manager.h"

#include "seat.h"
#include "types.h"

#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>


PendingManager* Manager::manager(const QDBusConnection& connection)
{
    PendingManager *ps = new PendingManager();
    ps->setInterface(createManager(connection));
    return ps;
}

ManagerPtr Manager::createManager(const QDBusConnection& connection)
{
    static QWeakPointer<Manager> s_manager;
    if (s_manager) {
        return s_manager.toStrongRef();
    }
    ManagerPtr s(new Manager(connection));
    s_manager = (s);
    return s;
}

Manager::Manager(const QDBusConnection& connection):
    OrgFreedesktopLogin1ManagerInterface("org.freedesktop.login1", "/org/freedesktop/login1", connection)
{

}
