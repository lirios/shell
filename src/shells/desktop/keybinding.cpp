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

#include "keybinding.h"
#include "keybinding_p.h"

/*
 * KeyBindingPrivate
 */

KeyBindingPrivate::KeyBindingPrivate()
    : QtWayland::wl_hawaii_key_binding()
    , q_ptr(0)
{
}

KeyBindingPrivate::~KeyBindingPrivate()
{
    wl_hawaii_key_binding_destroy(object());
}

void KeyBindingPrivate::hawaii_key_binding_triggered()
{
    Q_Q(KeyBinding);
    Q_EMIT q->triggered();
}

/*
 * KeyBinding
 */

KeyBinding::KeyBinding(quint32 key, quint32 modifiers, QObject *parent)
    : QObject(parent)
    , d_ptr(new KeyBindingPrivate())
{
    Q_D(KeyBinding);
    d->q_ptr = this;
    d->key = key;
    d->modifiers = modifiers;
}

KeyBinding::~KeyBinding()
{
    delete d_ptr;
}

#include "moc_keybinding.cpp"
