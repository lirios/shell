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
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes
import "private" as Private

Private.PopupBase {
    default property alias content: container.data

    id: root
    width: implicitWidth
    height: implicitHeight
    implicitWidth: Themes.Units.dp(300)

    Rectangle {
        id: container
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        color: Themes.Theme.palette.panel.backgroundColor
        radius: Themes.Units.dp(6)
        height: parent.height
        border.color: Themes.Theme.palette.rgba(Qt.darker(Themes.Theme.palette.panel.backgroundColor, 1.5), 0.3)
        border.width: Themes.Units.dp(1)
        antialiasing: true
        opacity: showing ? 1.0 : 0.0
        visible: opacity > 0.0

        Controls.PopupBehavior {
            anchors.fill: parent
            onDismissed: root.close()
        }
    }
}

