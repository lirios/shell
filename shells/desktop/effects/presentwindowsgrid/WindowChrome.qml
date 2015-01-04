/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import org.kde.plasma.core 2.0 as PlasmaCore
import "../.."

Item {
    property var window: parent

    signal clicked(var window)

    id: root
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        anchors.margins: -units.gridUnit * 0.25
        color: Theme.view.selectedBackgroundColor
        opacity: mouseArea.containsMouse ? 0.5 : 0.0
        radius: units.gridUnit * 0.2

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: units.shortDuration
            }
        }
    }

    Image {
        anchors {
            top: parent.top
            right: parent.right
            margins: -units.gridUnit * 0.25
        }
        source: "../../images/closebutton.png"
        z: 4
        width: units.iconSizes.smallMedium
        height: width
        smooth: true
        opacity: mouseArea.containsMouse ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: units.shortDuration
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: window.surface.destroySurface()
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked(window.parent)
    }
}
