/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Fluid.Controls 1.0

Rectangle {
    id: root

    property alias hovered: mouseArea.containsMouse
    signal clicked()

    Material.theme: Material.Dark

    width: Units.iconSizes.smallMedium
    height: width
    radius: width * 0.5
    border.color: Qt.rgba(1, 1, 1, 0.35)
    border.width: Units.gu(0.05)
    color: Material.dialogColor
    antialiasing: true

    Icon {
        anchors.centerIn: parent
        name: "navigation/close"
        color: "white"
        width: parent.width - Units.smallSpacing
        height: width
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
