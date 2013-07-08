/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

    padding {
        left: shadowSize + 4
        top: shadowSize + 4
        right: shadowSize + 4
        bottom: shadowSize + 4
    }

    property Component panel: Item {
        implicitWidth: 720
        implicitHeight: 480

        Rectangle {
            id: border
            anchors.fill: parent
            anchors.margins: shadowSize
            border.color: "#999"
            radius: 6
            color: "#cecece"
            visible: false
        }

        DropShadow {
            anchors.fill: border
            source: border
            radius: shadowSize
            samples: radius * 2
            fast: true
            spread: 0
            transparentBorder: true
        }
    }
}
