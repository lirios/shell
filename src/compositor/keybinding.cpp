/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include "keybinding.h"

KeyBinding::KeyBinding(struct ::wl_client *client, int id)
    : QtWaylandServer::wl_hawaii_key_binding(client, id)
    , m_modifiers(0)
    , m_key(0)
{
}

uint32_t KeyBinding::modifiers() const
{
    return m_modifiers;
}

void KeyBinding::setModifiers(uint32_t modifiers)
{
    m_modifiers = modifiers;
}

uint32_t KeyBinding::key() const
{
    return m_key;
}

void KeyBinding::setKey(uint32_t key)
{
    m_key = key;
}
