/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

Rectangle {
    property alias hovered: mouseArea.containsMouse
    signal clicked()

    id: root
    width: Themes.Units.iconSizes.smallMedium
    height: width
    radius: width * 0.5
    border.color: Qt.rgba(1, 1, 1, 0.35)
    border.width: Themes.Units.gu(0.05)
    gradient: Gradient {
        GradientStop { position: 0; color: "#666" }
        GradientStop { position: 1; color: "#222" }
    }
    antialiasing: true

    Components.Icon {
        anchors.centerIn: parent
        iconName: "window-close-symbolic"
        color: "white"
        width: parent.width - Themes.Units.smallSpacing
        height: width
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}

