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
    readonly property string title: source.surface.title
    readonly property bool active: source.focus
    property alias source: renderer.source
    readonly property real dropShadowExtents: Themes.Units.gridUnit
    readonly property real additionalWidth: dropShadowExtents
    readonly property real additionalHeight: dropShadowExtents + titleBar.height + frame.border.width

    signal close()
    signal minimize()
    signal maximize()

    id: root

    BorderImage {
        anchors {
            fill: parent
            margins: -dropShadowExtents
        }
        source: "graphics/dropshadow.sci"
        cache: true
        opacity: root.active ? 0.9 : 0.7
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
        }
        width: renderer.width + (border.width * 2)
        height: renderer.height + (border.width * 2) + titleBar.height
        border.color: Themes.Theme.palette.rgba(Themes.Theme.palette.window.secondaryColor, 0.5)
        border.width: 1
        z: 1

        WindowTitleBar {
            id: titleBar
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            title: root.title
            active: root.active
            height: Themes.Units.iconSizes.medium
            z: 0
            onClose: root.close()
            onMinimize: root.minimize()
            onMaximize: root.maximize()
        }

        SurfaceRenderer {
            id: renderer
            anchors {
                left: parent.left
                top: titleBar.bottom
            }
            width: source.surface.size.width
            height: source.surface.size.height
            z: 1
        }
    }
}
