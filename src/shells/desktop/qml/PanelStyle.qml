/****************************************************************************
 * This file is part of Desktop Shell.
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

import QtQuick 2.0
import QtGraphicalEffects 1.0
import Hawaii.Shell.Styles 0.1

Style {
    property int shadowSize: 8
    property color panelColor: "#f2111111"

    padding {
        bottom: shadowSize
    }

    property Component panel: Item {
        implicitHeight: __item.size + padding.bottom

        DropShadow {
            anchors.fill: background
            source: background
            horizontalOffset: 2
            verticalOffset: 2
            radius: shadowSize
            samples: radius * 2
            fast: true
            spread: 0
            transparentBorder: true
            color: Qt.rgba(0, 0, 0, 0.7)
        }

        Rectangle {
            id: background
            width: parent.width
            height: __item.size
            gradient: Gradient {
                GradientStop { position: 0; color: Qt.lighter(panelColor, 2.5) }
                GradientStop { position: 1; color: panelColor }
            }
            color: panelColor
            visible: false
        }
    }
}
