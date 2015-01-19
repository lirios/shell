/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtGraphicalEffects 1.0
import Hawaii.Themes 1.0 as Themes
import "../../components" as ShellComponents

Item {
    property var window: parent

    signal clicked(var window)

    id: root

    RectangularGlow {
        anchors.fill: parent
        glowRadius: 10
        spread: 0.2
        color: Themes.Theme.palette.view.selectedBackgroundColor
        cornerRadius: Themes.Units.gu(0.2)
        opacity: mouseArea.containsMouse ? 0.5 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: Themes.Units.shortDuration
            }
        }
    }

    ShellComponents.CloseButton {
        anchors {
            top: parent.top
            right: parent.right
            margins: -Themes.Units.gu(0.25)
        }
        z: 4
        opacity: mouseArea.containsMouse ? 1.0 : 0.0
        onClicked: window.child.surface.destroySurface()

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: Themes.Units.shortDuration
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        onClicked: root.clicked(window)
    }
}
