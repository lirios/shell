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
import QtQuick.Window 2.0
import GreenIsland 1.0
import FluidCore 1.0

Window {
    id: launcherContainer
    color: "transparent"

    // TODO: Define margins and padding in Fluid::Theme
    property real padding: 4

    // Tile size
    property alias tileSize: launcherView.tileSize

    // Alignment and orientation
    property int alignment: LauncherAlignment.Bottom
    property alias orientation: launcherView.orientation

    // Size
    property int size: {
        switch (alignment) {
        case LauncherAlignment.Left:
            return tileSize + frame.margins.right + (padding * 2);
        case LauncherAlignment.Right:
            return tileSize + frame.margins.left + (padding * 2);
        default:
            return tileSize + frame.margins.top + (padding * 2);
        }
    }

    // Number of items
    property alias count: launcherView.count

    Settings {
        id: settings
        schema: "org.hawaii.greenisland"
        group: "launcher"
        onValueChanged: loadSettings()
    }

    FrameSvgItem {
        id: frame
        anchors.fill: parent
        enabledBorders: {
            switch (alignment) {
            case LauncherAlignment.Left:
                return FrameSvgItem.RightBorder;
            case LauncherAlignment.Right:
                return FrameSvgItem.LeftBorder;
            case LauncherAlignment.Bottom:
                return FrameSvgItem.LeftBorder | FrameSvgItem.TopBorder | FrameSvgItem.RightBorder;
            }
        }
        imagePath: "widgets/background"
    }

    Item {
        id: launcher
        anchors.fill: frame

        LauncherView {
            id: launcherView
            anchors.fill: parent
            orientation: {
                switch (alignment) {
                case LauncherAlignment.Bottom:
                    return ListView.Horizontal;
                default:
                    return ListView.Vertical;
                }
            }
        }

        states: [
            State {
                name: "left"
                when: alignment === LauncherAlignment.Left

                PropertyChanges {
                    target: launcherView
                    anchors.topMargin: frame.margins.top + padding
                    anchors.rightMargin: frame.margins.right + padding
                    anchors.bottomMargin: frame.margins.top + padding
                }
            },
            State {
                name: "right"
                when: alignment === LauncherAlignment.Right

                PropertyChanges {
                    target: launcherView
                    anchors.leftMargin: frame.margins.left + padding
                    anchors.topMargin: frame.margins.top + padding
                    anchors.bottomMargin: frame.margins.top + padding
                }
            },
            State {
                name: "bottom"
                when: alignment === LauncherAlignment.Bottom

                PropertyChanges {
                    target: launcherView
                    anchors.leftMargin: frame.margins.left + padding
                    anchors.topMargin: frame.margins.top + padding
                    anchors.rightMargin: frame.margins.right + padding
                }
            }
        ]
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
