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

import QtQuick 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Core 1.0
import Fluid.Controls 1.0
import Liri.Platform 1.0

Object {
    id: platformExtensions

    property color decorationColor
    property var window: null

    Component.onCompleted: {
        if (decorationColor != "#000000")
            updateDecorationColor()
    }

    onDecorationColorChanged: {
        if (decorationColor != "#000000") {
            updateDecorationColor();
        }
    }

    function updateDecorationColor() {
        decorations.backgroundColor = decorationColor
        decorations.iconColor = Utils.lightDark(decorationColor, FluidStyle.iconColorLight,
                FluidStyle.iconColorDark)
        decorations.textColor = Utils.lightDark(decorationColor, Qt.rgba(0,0,0,0.87),
                Qt.rgba(1,1,1,1))
        decorations.update()
    }

    WindowDecorations {
        id: decorations
        window: platformExtensions.window
    }
}
