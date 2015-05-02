/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
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

#ifndef HAWAIITHEME_P_H
#define HAWAIITHEME_P_H

#include <qpa/qplatformtheme_p.h>

#include "hawaiitheme.h"
#include "qgsettings.h"

class HawaiiThemePrivate : public QPlatformThemePrivate
{
public:
    HawaiiThemePrivate();
    ~HawaiiThemePrivate();

    void readPalette(QPalette *pal);
    void refresh();

    static bool readColor(QPalette *pal, QPalette::ColorRole role, const QVariant &value);

    ResourceHelper resources;
    Hawaii::QGSettings *settings;
};

#endif // HAWAIITHEME_P_H
