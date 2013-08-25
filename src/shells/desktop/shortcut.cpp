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

#include "shortcut.h"
#include "shortcut_p.h"
#include "desktopshell.h"
#include "keybinding.h"

/*
 * ShortcutPrivate
 */

ShortcutPrivate::ShortcutPrivate()
    : binding(0)
{
}

ShortcutPrivate::~ShortcutPrivate()
{
    delete binding;
}

void ShortcutPrivate::qtSequence2Wayland(const QKeySequence &s, quint32 *keys, quint32 *mods)
{
    if (!keys && !mods)
        return;

    quint32 decodedKeys = 0;
    quint32 decodedMods = 0;

    for (int i = 0; i < s.count(); i++) {
        int seq = s[i];

        switch (seq) {
        case Qt::ControlModifier:
            decodedMods |= MODIFIER_CTRL;
            break;
        case Qt::AltModifier:
            decodedMods |= MODIFIER_ALT;
            break;
        case Qt::MetaModifier:
            decodedMods |= MODIFIER_SUPER;
            break;
        case Qt::ShiftModifier:
            decodedMods |= MODIFIER_SHIFT;
            break;
        }
        if (decodedMods > 0)
            continue;

        int k = 0;
        while (keyTable[k]) {
            if (seq == (int)keyTable[k]) {
                decodedKeys |= keyTable[k + 1];
                break;
            }
            k += 2;
        }
    }

    if (keys)
        *keys = decodedKeys;
    if (mods)
        *mods = decodedMods;
}

/*
 * Shortcut
 */

Shortcut::Shortcut(QObject *parent)
    : QObject(parent)
    , d_ptr(new ShortcutPrivate())
{
}

Shortcut::~Shortcut()
{
    delete d_ptr;
}

QVariant Shortcut::key() const
{
    Q_D(const Shortcut);
    return d->sequence;
}

void Shortcut::setKey(const QVariant &value)
{
    Q_D(Shortcut);

    QKeySequence sequence = value.value<QKeySequence>();

    if (d->sequence != sequence) {
        d->sequence = sequence;

        // Decode key sequence
        quint32 keys, modifiers;
        d->qtSequence2Wayland(sequence, &keys, &modifiers);

        // Create a key binding
        delete d->binding;
        d->binding = DesktopShell::instance()->addKeyBinding(keys, modifiers);
        connect(d->binding, SIGNAL(triggered()), this, SIGNAL(triggered()));

        Q_EMIT keyChanged();
    }
}

#include "moc_shortcut.cpp"
