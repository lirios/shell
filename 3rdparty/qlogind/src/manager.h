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

#ifndef MANAGER_H
#define MANAGER_H

#include <QDBusConnection>

#include "orgfreedesktoplogin1Manager.h"
#include "pendinginterface.h"


#include "qlogind_export.h"

class Manager;

typedef PendingInterface<Manager> PendingManager;
typedef QSharedPointer<Manager> ManagerPtr;

class QLOGIND_EXPORT Manager : public OrgFreedesktopLogin1ManagerInterface
{
    Q_OBJECT
public:
    static PendingManager* manager(const QDBusConnection &connection = QDBusConnection::systemBus());

protected:
    static ManagerPtr createManager(const QDBusConnection &connection);
    Manager(const QDBusConnection &connection);
};


#endif // USER_H
