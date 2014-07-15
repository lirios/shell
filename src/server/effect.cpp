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

#include "effect.h"
#include "shell.h"
#include "binding.h"

Effect::Effect()
{
    wl_event_loop *loop = wl_display_get_event_loop(Shell::instance()->compositor()->wl_display);
    wl_event_loop_add_idle(loop, [](void *data) { Shell::instance()->registerEffect(static_cast<Effect *>(data)); }, this);
}

Effect::~Effect()
{
    for (Bindings::value_type v: m_bindings) {
        delete v.second;
    }
}

void Effect::addSurface(ShellSurface *surf)
{
    addedSurface(surf);
}

void Effect::removeSurface(ShellSurface *surf)
{
    removedSurface(surf);
}

Binding *Effect::binding(const std::string &name)
{
    return m_bindings[name];
}

void Effect::addBinding(const std::string &n, Binding *b)
{
    m_bindings[n] = b;
}



Effect::Settings::Settings()
                : ::Settings("effects")
{
}

std::list<Option> Effect::Settings::options() const
{
    std::list<Option> list;
    list.push_back(Option::integer("enabled"));

    return list;
}
