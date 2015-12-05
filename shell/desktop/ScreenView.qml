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
import QtGraphicalEffects 1.0
import GreenIsland 1.0 as GreenIsland
import Hawaii.Themes 1.0 as Themes
import org.hawaiios.misc 0.1 as Misc
import ".."
import "../components"
import "../indicators"

Item {
    property var layers: QtObject {
        //readonly property alias panels: shellLoader.item
        readonly property alias workspaces: workspacesLayer
        readonly property alias fullScreen: fullScreenLayer
        readonly property alias overlays: overlaysLayer
        readonly property alias notifications: notificationsLayer
    }

    readonly property alias currentWorkspace: workspacesLayer.currentWorkspace

    readonly property var panel: shellLoader.item ? shellLoader.item.panel : null

    id: screenView
    state: "normal"
    states: [
        State {
            name: "normal"
        },
        State {
            name: "windowsSwitcher"
            PropertyChanges { target: windowSwitcherLoader; active: true }
            //StateChangeScript { script: disableInput() }
        }
    ]

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

    // Background is below everything
    Background {
        id: backgroundLayer
        anchors.fill: parent
        z: 0
    }

    // Desktop is only above background
    Desktop {
        id: desktopLayer
        anchors.fill: parent
        z: 1
    }

    // Workspaces
    WorkspacesView {
        id: workspacesLayer
        anchors.fill: parent
        z: 2
    }

    // Panels
    Loader {
        id: shellLoader
        anchors.fill: parent
        active: primary
        sourceComponent: Shell {
            onPanelHeightChanged: setAvailableGeometry(panel.height)
        }
        z: 3
        visible: status == Loader.Ready
    }

    // Full screen windows can cover application windows and panels
    Rectangle {
        id: fullScreenLayer
        anchors.fill: parent
        color: "black"
        z: 10
        opacity: children.length > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: Themes.Units.mediumDuration
            }
        }
    }

    // Overlays are above the panel
    Overlay {
        id: overlaysLayer
        anchors.centerIn: parent
        z: 5
    }

    // Notifications are behind the panel
    Item {
        id: notificationsLayer
        anchors.fill: parent
        z: 5
    }

    // Key events filter
    GreenIsland.KeyEventFilter {
        id: keyFilter

        Keys.onPressed: {
            //console.debug("Key pressed:", event.key);

            // Windows switcher
            if (event.modifiers & Qt.MetaModifier) {
                console.log("key", event.key, Qt.Key_Meta, Qt.Key_Tab, Qt.Key_Backtab);
                if (event.key === Qt.Key_Tab || event.key === Qt.Key_Backtab) {
                    if (!windowSwitcherLoader.active) {
                    //if (screenView.state != "windowsSwitcher" && currentWorkspace.children.length >= 2) {
                        // Activate only when two or more windows are available
                        //screenView.state = "windowsSwitcher";
                        windowSwitcherLoader.active = true;
                        event.accepted = true;
                        return;
                    }
                }
            }
        }
    }

    // Windows switcher
    Loader {
        id: windowSwitcherLoader
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        active: false
        asynchronous: true
        source: "../WindowSwitcher.qml"
        z: 1000

        Connections {
            target: windowSwitcherLoader.item
            onClosed: windowSwitcherLoader.active = false
        }
    }

    function setAvailableGeometry(h) {
        // Set available geometry so that windows are maximized properly
        var geometry = output.geometry;
        geometry.height -= h;
        output.availableGeometry = geometry;
        console.debug("Available geometry for", output.model, "is:", output.availableGeometry);
    }
}
