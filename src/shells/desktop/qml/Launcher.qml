/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import GreenIsland 1.0
import Hawaii.Shell.Styles 0.1
import FluidCore 0.2 as FluidCore

StyledItem {
    id: launcherContainer

    // Tile size
    property real tileSize: launcherView.tileSize

    // Alignment and orientation
    property int alignment: LauncherAlignment.Bottom
    property alias orientation: launcherView.orientation

    // Size
    property int size: tileSize + __style.padding.left + __style.padding.top + __style.padding.right + __style.padding.bottom

    // Number of items
    property alias count: launcherView.count

    style: Qt.createComponent("LauncherStyle.qml", launcherContainer)

    FluidCore.Settings {
        id: settings
        schema: "org.hawaii.greenisland"
        group: "launcher"
        onValueChanged: loadSettings()
    }

    LauncherView {
        id: launcherView
        anchors {
            fill: parent
            leftMargin: __style.padding.left
            topMargin: __style.padding.top
            rightMargin: __style.padding.right
            bottomMargin: __style.padding.bottom
        }
        orientation: {
            switch (alignment) {
            case LauncherAlignment.Bottom:
                return ListView.Horizontal;
            default:
                return ListView.Vertical;
            }
        }
    }

    Component.onCompleted: loadSettings()

    function loadSettings() {
        var alignmentVal = settings.value("alignment");

        if (alignmentVal === "left")
            alignment = LauncherAlignment.Left;
        else if (alignmentVal === "right")
            alignment = LauncherAlignment.Right;
        else
            alignment = LauncherAlignment.Bottom;
    }
}
