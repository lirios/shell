/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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
import GreenIsland 1.0
import Fluid.Core 1.0
import Fluid.Controls 1.0

Item {
    property var clientWindow: null
    property alias container: container
    readonly property real additionalWidth: shadow.extents + (frame.border.width * 2)
    readonly property real additionalHeight: shadow.extents + titleBar.height + (frame.border.height * 2)

    id: root

    BorderImage {
        readonly property real extents: 20

        id: shadow
        anchors.fill: parent
        anchors.margins: -extents
        source: "graphics/dropshadow.sci"
        cache: true
        opacity: clientWindow && clientWindow.activated ? 0.9 : 0.7
        smooth: true
        z: 0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: Units.shortDuration
            }
        }
    }

    Rectangle {
        id: frame
        anchors {
            left: parent.left
            top: parent.top
            margins: shadow.extents
        }
        width: container.width + (border.width * 2)
        height: container.height + (border.width * 2) + titleBar.height
        border.color: Utils.alpha(Material.dialogColor, 0.5)
        border.width: 1
        color: "transparent"
        z: 1

        WindowTitleBar {
            id: titleBar
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            title: clientWindow ? clientWindow.title : ""
            active: clientWindow && clientWindow.active
            height: Units.iconSizes.medium
            z: 0
            onClicked: if (clientWindow) clientWindow.activate()
            onMoving: if (clientWindow) clientWindow.position = Qt.point(x, y)
            onClose: if (clientWindow) clientWindow.close()
            onMinimize: if (clientWindow) clientWindow.minimize()
            onMaximize: if (clientWindow) clientWindow.maximize()
        }

        Item {
            id: container
            anchors {
                left: parent.left
                top: titleBar.bottom
            }
            width: clientWindow ? clientWindow.size.width : 0
            height: clientWindow ? clientWindow.size.height : 0
            z: 1
        }
    }
}
