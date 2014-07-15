/*
 * Copyright 2013-2014  Giulio Camuffo <giuliocamuffo@gmail.com>
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

#ifndef SCROLLDOWN_H
#define SCROLLDOWN_H

#include <list>

#include <wayland-server.h>

#include "interface.h"

class Instance;

class Dropdown : public Interface
{
public:
    Dropdown();

    std::list<wl_client *> boundClients() const;

private:
    void bind(wl_client *client, uint32_t version, uint32_t id);

    std::list<wl_client *> m_instances;

    friend Instance;
};

#endif
