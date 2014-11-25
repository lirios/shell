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
        property bool entered: false
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
            id: topImage
            anchors.fill: parent
            source: "/images/corner-ripple-ltr.png"
            visible: hotCorner.type == Item.Top
        }
        Image {
            id: bottomImage
            anchors.fill: parent
            source: "/images/corner-ripple-ltr.png"
            visible: hotCorner.type == Item.Bottom
        }
        Image {
            id: leftImage
            anchors.fill: parent
            source: "/images/corner-ripple-ltr.png"
            visible: hotCorner.type == Item.Left
        }
        Image {
            id: rightImage
            anchors.fill: parent
            source: "/images/corner-ripple-rtl.png"
            visible: hotCorner.type == Item.Right
        }

        Image {
            id: topLeftImage
            anchors.fill: parent
            source: "/images/corner-ripple-ltr.png"
            visible: hotCorner.type == Item.TopLeft
        }
        Image {
            id: topRightImage
            anchors.fill: parent
            source: "/images/corner-ripple-rtl.png"
            visible: hotCorner.type == Item.TopRight
        }
        Image {
            id: bottomLeftImage
            anchors.fill: parent
            source: "/images/corner-ripple-ltr.png"
            transform: Rotation { origin.x: width / 2; origin.y: height / 2; angle: -90 }
            visible: hotCorner.type == Item.BottomLeft
        }
        Image {
            id: bottomRightImage
            anchors.fill: parent
            source: "/images/corner-ripple-rtl.png"
            transform: Rotation { origin.x: width / 2; origin.y: height / 2; angle: 90 }
            visible: hotCorner.type == Item.BottomRight
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onEntered: rect.opacity = 1.0
        onExited: rect.opacity = 0.0
    }

    MouseArea {
        id: surroundingArea
        anchors.left: parent.left
        anchors.top: parent.top
        width: 3
        height: 3
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onExited: {
            __priv.entered = false;
        }

        MouseArea {
            id: cornerArea
            anchors.left: parent.left
            anchors.top: parent.top
            width: 1
            height: 1
            acceptedButtons: Qt.NoButton
            hoverEnabled: true
            onEntered: {
                // Mouse entered this corner, do we have to trigger?
                /*
                var curtime = new Date().getTime();
                if (curtime - __priv.lastTrigger > __priv.threshold) {
                    hotCorner.triggered();
                    __priv.lastTrigger = new Date().getTime();
                }
                */
                if (!__priv.entered) {
                    __priv.entered = true;
                    hotCorner.triggered();
                }
            }
            onExited: {
                // Save last trigger only when it exits to avoid
                // triggering when the mouse is still in the area
                //__priv.lastTrigger = new Date().getTime();
                __priv.entered = false;
            }
        }
    }
}
