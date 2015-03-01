/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import GreenIsland 1.0
import Hawaii.Themes 1.0 as Themes

Item {
    signal topLeftTriggered()
    signal topRightTriggered()

    signal bottomLeftTriggered()
    signal bottomRightTriggered()

    id: root

    // Redirect events
    Connections {
        target: _greenisland_output
        onHotSpotTriggered: {
            switch (hotSpot) {
            case Output.TopLeftHotSpot:
                root.topLeftTriggered();
                break;
            case Output.TopRightHotSpot:
                root.topRightTriggered();
                break;
            case Output.BottomLeftHotSpot:
                root.bottomLeftTriggered();
                break;
            case Output.BottomRightHotSpot:
                root.bottomRightTriggered();
                break;
            default:
                break;
            }
        }
    }

    // Top-left corner
    Image {
        id: topLeftCorner
        anchors {
            left: parent.left
            top: parent.top
        }
        source: "../images/corner-ripple-ltr.png"
        sourceSize.width: width
        sourceSize.height: height
        width: Themes.Units.dp(32)
        height: width
        opacity: 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: Themes.Units.shortDuration
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            hoverEnabled: true
            onEntered: parent.opacity = 1.0
            onExited: parent.opacity = 0.0
        }
    }

    // Top-right corner
    Image {
        id: topRightCorner
        anchors {
            right: parent.right
            top: parent.top
        }
        source: "../images/corner-ripple-rtl.png"
        sourceSize.width: width
        sourceSize.height: height
        width: Themes.Units.dp(32)
        height: width
        opacity: 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: Themes.Units.shortDuration
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            hoverEnabled: true
            onEntered: parent.opacity = 1.0
            onExited: parent.opacity = 0.0
        }
    }

    // Bottom-left corner
    Image {
        id: bottomLeftCorner
        anchors {
            left: parent.left
            bottom: parent.bottom
        }
        source: "../images/corner-ripple-ltr.png"
        sourceSize.width: width
        sourceSize.height: height
        width: Themes.Units.dp(32)
        height: width
        transform: Rotation { origin.x: width / 2; origin.y: height / 2; angle: -90 }
        opacity: 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: Themes.Units.shortDuration
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            hoverEnabled: true
            onEntered: parent.opacity = 1.0
            onExited: parent.opacity = 0.0
        }
    }

    // Bottom-right corner
    Image {
        id: bottomRightCorner
        anchors {
            right: parent.right
            bottom: parent.bottom
        }
        source: "../images/corner-ripple-rtl.png"
        sourceSize.width: width
        sourceSize.height: height
        width: Themes.Units.dp(32)
        height: width
        transform: Rotation { origin.x: width / 2; origin.y: height / 2; angle: 90 }
        opacity: 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: Themes.Units.shortDuration
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            hoverEnabled: true
            onEntered: parent.opacity = 1.0
            onExited: parent.opacity = 0.0
        }
    }
}
