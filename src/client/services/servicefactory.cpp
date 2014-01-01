/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QDebug>

#include "servicefactory.h"

Q_GLOBAL_STATIC(ServiceFactory, s_serviceFactory)

QObject *ServiceFactory::createService(const QString &name, QObject *parent)
{
    // Is the factory registered?
    if (!s_serviceFactory->m_factories.contains(name))
        return 0;

    Factory factory = s_serviceFactory->m_factories.value(name);
    return factory(parent);
}

void ServiceFactory::registerNamedFactory(const QString &name, Factory factory)
{
    // Sanity checks
    if (name.isEmpty()) {
        qWarning() << "Registering unnamed factories is forbidden";
        return;
    }
    if (s_serviceFactory->m_factories.contains(name)) {
        qWarning() << "Factory for service" << name << "already registered";
        return;
    }

    // Append the service
    s_serviceFactory->m_factories[name] = factory;
}
