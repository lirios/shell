/*
 * Copyright 2013  Giulio Camuffo <giuliocamuffo@gmail.com>
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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INTERFACE_H
#define INTERFACE_H

#include <list>
#include <type_traits>

class Interface;

class Object
{
public:
    Object();
    virtual ~Object();

    void addInterface(Interface *iface);
    void destroy();

    template <class T>
    T *findInterface() const;

private:
    std::list<Interface *> m_ifaces;
    bool m_deleting;
};

class Interface
{
public:
    Interface();
    virtual ~Interface() {}

    Object *object() { return m_obj; }

protected:
    virtual void added() {}

private:
    Object *m_obj;

    friend class Object;
};


template <class T>
T *Object::findInterface() const
{
    static_assert(std::is_base_of<Interface, T>::value, "T is not derived from Interface.");
    for (Interface *iface: m_ifaces) {
        if (T *t = dynamic_cast<T *>(iface)) {
            return t;
        }
    }
    return nullptr;
}

#endif
