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

#include "shellmanager.h"
#include "shortcut.h"
#include "shortcut_p.h"
#include "keybinding.h"

#define CHECK_MODIFIER(encoded, decoded, qtmod) \
    if ((encoded & qtmod) == qtmod) { \
        decoded |= qtModifier2Wayland(qtmod); \
        encoded &= ~qtmod; \
    }

static int qtModifier2Wayland(int modifier)
{
    switch (modifier) {
    case Qt::ControlModifier:
        return MODIFIER_CTRL;
    case Qt::AltModifier:
        return MODIFIER_ALT;
    case Qt::MetaModifier:
        return MODIFIER_SUPER;
    case Qt::ShiftModifier:
        return MODIFIER_SHIFT;
    default:
        break;
    }

    return 0;
}

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

    // TODO: We should create a binding for each sequence
    for (int i = 0; i < s.count(); i++) {
        int seq = s[i];

        CHECK_MODIFIER(seq, decodedMods, Qt::ControlModifier);
        CHECK_MODIFIER(seq, decodedMods, Qt::AltModifier);
        CHECK_MODIFIER(seq, decodedMods, Qt::MetaModifier);
        CHECK_MODIFIER(seq, decodedMods, Qt::ShiftModifier);

        int k = 0;
        while (keyTable[k]) {
            if ((seq & (int)keyTable[k]) == (int)keyTable[k]) {
                decodedKeys |= keyTable[k + 1];
                seq &= ~(int)keyTable[k];
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
        d->binding = ShellManager::instance()->controller()->addKeyBinding(keys, modifiers);
        connect(d->binding, SIGNAL(triggered()), this, SIGNAL(triggered()));

        Q_EMIT keyChanged();
    }
}

#include "moc_shortcut.cpp"
