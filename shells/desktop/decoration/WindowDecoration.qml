/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
        opacity: clientWindow && clientWindow.active ? 0.9 : 0.7
        smooth: true
        z: 0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: Themes.Units.shortDuration
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
        border.color: Themes.Theme.palette.rgba(Themes.Theme.palette.window.secondaryColor, 0.5)
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
            height: Themes.Units.iconSizes.medium
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
