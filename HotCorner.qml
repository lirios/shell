/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0

Item {
    property int type: Item.TopLeft

    signal triggered()

    id: hotCorner
    // FIXME: Size depends on DPIs
    width: 32
    height: 32

    QtObject {
        id: __priv

        property int threshold: 5
        property double lastTrigger

        Component.onCompleted: lastTrigger = new Date().getTime()
    }

    Item {
        id: rect
        anchors.fill: parent
        opacity: 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 250
            }
        }

        Image {
            id: topLeftImage
            anchors.fill: parent
            source: "../images/corner-ripple-ltr.png"
            visible: hotCorner.type == Item.TopLeft
        }
        Image {
            id: topRightImage
            anchors.fill: parent
            source: "../images/corner-ripple-rtl.png"
            visible: hotCorner.type == Item.TopRight
        }
        Image {
            id: bottomLeftImage
            anchors.fill: parent
            source: "../images/corner-ripple-ltr.png"
            transform: Rotation { origin.x: width / 2; origin.y: height / 2; angle: -90 }
            visible: hotCorner.type == Item.BottomLeft
        }
        Image {
            id: bottomRightImage
            anchors.fill: parent
            source: "../images/corner-ripple-rtl.png"
            transform: Rotation { origin.x: width / 2; origin.y: height / 2; angle: 90 }
            visible: hotCorner.type == Item.BottomRight
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: rect.opacity = 1.0
        onExited: rect.opacity = 0.0
        onContainsMouseChanged: {
            if (containsMouse) {
                // Mouse entered this corner, do we have to trigger?
                var curtime = new Date().getTime();
                if (curtime - __priv.lastTrigger > __priv.threshold)
                    hotCorner.triggered();
            }
        }
    }
}
