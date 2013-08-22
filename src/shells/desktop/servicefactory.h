/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SERVICEFACTORY_H
#define SERVICEFACTORY_H

#include <QtCore/QObject>
#include <QtCore/QHash>

class ServiceFactory
{
public:
    template<class T> static void registerFactory();

    static QObject *createService(const QString &name);

private:
    typedef QObject *(*Factory)();

    static void registerNamedFactory(const QString &name, Factory factory);
    template<class T> static QObject *createFactory() { return new T(); }

    QHash<QString, Factory> m_factories;
};

template<class T>
void ServiceFactory::registerFactory()
{
    static_assert(std::is_base_of<QObject, T>::value, "Must inherit from QObject");
    registerNamedFactory(QString::fromUtf8(T::name()), createFactory<T>);
}

#endif // SERVICEFACTORY_H
