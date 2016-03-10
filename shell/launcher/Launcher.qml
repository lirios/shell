/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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
import Fluid.Ui 1.0 as FluidUi
import org.hawaiios.launcher 0.1 as CppLauncher

Item {
    property real iconSize
    property real itemSize
    readonly property real itemPadding: FluidUi.Units.smallSpacing * 2
    property alias orientation: listView.orientation
    readonly property alias currentItem: listView.currentItem

    id: launcher
    states: [
        State {
            name: "horizontal"
            when: listView.orientation == ListView.Horizontal

            PropertyChanges {
                target: listView
                width: listView.contentWidth
                height: launcher.height
            }
        },
        State {
            name: "vertical"
            when: listView.orientation == ListView.Vertical

            PropertyChanges {
                target: listView
                width: launcher.width
                height: listView.contentHeight
            }
        }
    ]

    ListView {
        id: listView
        anchors.centerIn: parent
        spacing: FluidUi.Units.smallSpacing
        interactive: false
        add: Transition {
            NumberAnimation { properties: "scale"; from: 0.1; to: 1.0; duration: FluidUi.Units.shortDuration }
        }
        populate: Transition {
            NumberAnimation { properties: "scale"; from: 0.1; to: 1.0; duration: FluidUi.Units.longDuration }
        }
        model: CppLauncher.LauncherModel {
            id: launcherModel
            applicationManager: hawaiiCompositor.applicationManager
        }
        delegate: LauncherDelegate {}
    }
}
