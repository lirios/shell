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

#ifndef KEYBINDING_H
#define KEYBINDING_H

#include <QtCore/QObject>

class DesktopShell;
class KeyBindingPrivate;

class KeyBinding : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(KeyBinding)
    Q_DECLARE_PRIVATE(KeyBinding)
public:
    ~KeyBinding();

Q_SIGNALS:
    void triggered();

private:
    friend class DesktopShell;

    KeyBinding(quint32 key, quint32 modifiers, QObject *parent = 0);

    KeyBindingPrivate *const d_ptr;
};

#endif // KEYBINDING_H
