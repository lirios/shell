/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2010-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtGui/QFont>

#include "hawaiitheme.h"
#include "hawaiitheme_p.h"
#include "hintssettings.h"

HawaiiTheme::HawaiiTheme()
    : QPlatformTheme(new HawaiiThemePrivate())
{
    Q_D(HawaiiTheme);
    d->refresh();
}

bool HawaiiTheme::usePlatformNativeDialog(DialogType type) const
{
    return false;
}

QPlatformDialogHelper *HawaiiTheme::createPlatformDialogHelper(DialogType type) const
{
    return 0;
}

const QPalette *HawaiiTheme::palette(Palette type) const
{
    Q_D(const HawaiiTheme);
    return d->resources.palettes[type];
}

const QFont *HawaiiTheme::font(Font type) const
{
    Q_D(const HawaiiTheme);
    return d->resources.fonts[type];
}

QVariant HawaiiTheme::themeHint(ThemeHint hint) const
{
    Q_D(const HawaiiTheme);

    QVariant value = d->hints->themeHint(hint);
    if (value.isValid())
        return value;
    return QPlatformTheme::themeHint(hint);
}
