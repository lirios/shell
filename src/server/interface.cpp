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

#include "interface.h"

Object::Object()
      : m_deleting(false)
{
}

Object::~Object()
{
    m_deleting = true;
    for (Interface *iface: m_ifaces) {
        delete iface;
    }
}

void Object::addInterface(Interface *iface)
{
    m_ifaces.push_back(iface);
    iface->m_obj = this;
    iface->added();
}

void Object::destroy()
{
    if (!m_deleting) {
        delete this;
    }
}

Interface::Interface()
         : m_obj(nullptr)
{
}
