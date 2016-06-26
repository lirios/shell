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
import QtGraphicalEffects 1.0
import Qt.labs.controls 1.0
import Qt.labs.controls.material 1.0
import GreenIsland 1.0 as GreenIsland
import Fluid.Ui 1.0 as FluidUi
import org.hawaiios.misc 0.1 as Misc
import ".."
import "../components"
import "../indicators"

Item {
    property var layers: QtObject {
        readonly property alias workspaces: workspacesLayer
        readonly property alias fullScreen: fullScreenLayer
        readonly property alias overlays: overlaysLayer
        readonly property alias notifications: notificationsLayer
    }

    readonly property alias currentWorkspace: workspace //workspacesLayer.currentItem
    readonly property var panel: shellLoader.item ? shellLoader.item.panel : null
    readonly property alias windowSwitcher: windowSwitcher

    id: screenView

    Material.theme: Material.Dark

    /*
     * Hot corners
     */

    HotCorners {
        id: hotCorners
        anchors.fill: parent
        z: 2000
        onTopLeftTriggered: workspacesLayer.selectPrevious()
        onTopRightTriggered: workspacesLayer.selectNext()
        onBottomLeftTriggered: currentWorkspace.state = "present";
    }

    /*
     * Workspace
     */

    // Background image or color
    Background {
        id: backgroundLayer
        anchors.fill: parent
        z: 0
    }

    // Desktop applets
    Desktop {
        id: desktopLayer
        anchors.fill: parent
        z: 1
    }

    // Dim desktop in present mode
    Rectangle {
        anchors.fill: parent
        z: 2
        color: "black"
        opacity: workspace.state == "present" ? 0.7 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: 250
            }
        }
    }

    // Workspaces
    WorkspacesView {
        id: workspacesLayer
        anchors.fill: parent
        z: 5
    }

    // FIXME: Temporary workaround to make keyboard input work,
    // apparently SwipeView captures input. An Item instead make it work.
    Workspace {
        id: workspace
        anchors.fill: parent
        z: 6
    }

    // Panels
    Loader {
        id: shellLoader
        anchors.fill: parent
        asynchronous: true
        active: primary
        sourceComponent: Shell {
            opacity: workspace.state == "present" ? 0.0 : 1.0

            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 250
                }
            }
        }
        z: 10
    }

    // Full screen windows can cover application windows and panels
    Rectangle {
        id: fullScreenLayer
        anchors.fill: parent
        color: "black"
        z: 20
        opacity: children.length > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: FluidUi.Units.mediumDuration
            }
        }
    }

    // Overlays are above the panel
    Overlay {
        id: overlaysLayer
        anchors.centerIn: parent
        z: 10
    }

    // Notifications are behind the panel
    Item {
        id: notificationsLayer
        anchors.fill: parent
        z: 10
    }

    // Windows switcher
    WindowSwitcher {
        id: windowSwitcher
        x: (output.availableGeometry.width - width) / 2
        y: (output.availableGeometry.height - height) / 2
    }
}
