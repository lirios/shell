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

#ifndef KEYBINDING_H
#define KEYBINDING_H

#include "qwayland-server-hawaii.h"

class KeyBinding : public QtWaylandServer::hawaii_key_binding
{
public:
    KeyBinding(struct ::wl_client *client, int id);

    uint32_t modifiers() const;
    void setModifiers(uint32_t modifiers);

    uint32_t key() const;
    void setKey(uint32_t key);

private:
    uint32_t m_modifiers;
    uint32_t m_key;
};

typedef QList<KeyBinding *> KeyBindings;

#endif // KEYBINDING_H
