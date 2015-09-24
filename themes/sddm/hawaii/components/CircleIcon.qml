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
import Hawaii.Themes 1.0 as Themes
import Hawaii.Components 1.0 as Components

Item {
    property alias iconSize: icon.width
    property alias iconName: icon.iconName

    width: iconSize * 1.5
    height: width

    Rectangle {
        id: container
        anchors.fill: parent
        color: Themes.Theme.palette.panel.backgroundColor
        border.color: Themes.Theme.palette.rgba(Qt.darker(color, 1.3), 0.5)
        border.width: Themes.Units.dp(1)
        radius: width / 2
        antialiasing: true
    }

    Components.Icon {
        id: icon
        anchors.centerIn: parent
        iconName: "avatar-default-symbolic"
        height: width
        color: "white"
    }
}
