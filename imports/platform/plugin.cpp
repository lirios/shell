/****************************************************************************
 * This file is part of Liri Shell.
 *
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include "plugin.h"

#include "windowdecorations.h"

void PlatformPlugin::registerTypes(const char *uri)
{
    // @uri Liri.Platform
    Q_ASSERT(uri == QStringLiteral("Liri.Platform"));

    qmlRegisterType<WindowDecorations>(uri, 1, 0, "WindowDecorations");
}
