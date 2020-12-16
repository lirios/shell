/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
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
import QtWayland.Compositor 1.0 as QtWaylandCompositor
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects
import Liri.WaylandServer 1.0 as WS
import Liri.Shell 1.0 as LS
import ".."
import "../components"
import "../indicators"
import "../notifications"

Item {
    id: desktop

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    readonly property var layers: QtObject {
        readonly property alias fullScreen: fullScreenLayer
        readonly property alias notifications: notificationsLayer
    }

    readonly property alias shell: shellLoader.item
    readonly property alias currentWorkspace: workspacesView.currentWorkspace
    readonly property var panel: shell ? shell.panel : null
    readonly property alias windowSwitcher: windowSwitcher

    /*
     * Workspace
     */

    WorkspacesView {
        id: workspacesView
    }

    Loader {
        id: notificationsLayer
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            topMargin: FluidControls.Units.largeSpacing * 3
            bottomMargin: 56 + FluidControls.Units.smallSpacing
        }
        active: output.primary
        sourceComponent: Notifications {}
        width: FluidControls.Units.gu(24) + (2 * FluidControls.Units.smallSpacing)
    }

    // Panels
    Loader {
        id: shellLoader
        anchors.fill: parent
        active: output.primary
        sourceComponent: Shell {
            opacity: currentWorkspace.state == "present" ? 0.0 : 1.0
            visible: opacity > 0.0

            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                    duration: 250
                }
            }
        }
    }

    // Full screen windows can cover application windows and panels
    Rectangle {
        id: fullScreenLayer
        anchors.fill: parent
        color: "black"
        opacity: children.length > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: FluidControls.Units.mediumDuration
            }
        }
    }

    // Windows switcher
    WindowSwitcher {
        id: windowSwitcher
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
    }

    /*
     * Methods
     */

    function selectPreviousWorkspace() {
        workspacesView.selectPrevious();
    }

    function selectNextWorkspace() {
        workspacesView.selectNext();
    }

    function selectWorkspace(num) {
        workspacesView.select(num);
    }
}
