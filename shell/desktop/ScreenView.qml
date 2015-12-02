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


    /*
     * Hot corners
     */

    HotCorners {
        id: hotCorners
        anchors.fill: parent
        z: 2000
        onTopLeftTriggered: workspacesLayer.selectPrevious()
        onTopRightTriggered: workspacesLayer.selectNext()
        onBottomLeftTriggered: currentWorkspace.effects.toggle("PresentWindowsGrid")
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

    function setAvailableGeometry(h) {
        // Set available geometry so that windows are maximized properly
        var geometry = output.geometry;
        geometry.height -= h;
        output.availableGeometry = geometry;
        console.debug("Available geometry for", output.model, "is:", output.availableGeometry);
    }

    Component.onCompleted: {
        // Create default 4 workspaces
        var i;
        for (i = 0; i < 4; i++)
            workspacesLayer.add();
        workspacesLayer.select(0);
    }
}
