/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef KEYBINDING_P_H
#define KEYBINDING_P_H

#include "qwayland-desktop.h"

class KeyBindingPrivate : public QtWayland::hawaii_key_binding
{
    Q_DECLARE_PUBLIC(KeyBinding)
public:
    KeyBindingPrivate();
    ~KeyBindingPrivate();

    KeyBinding *q_ptr;
    quint32 key;
    quint32 modifiers;

protected:
    virtual void hawaii_key_binding_triggered();
};

#endif // KEYBINDING_P_H
