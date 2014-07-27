/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import ".."

Item {
    property string name
    property int number
    property bool primary

    readonly property alias workspacesView: workspacesLayer
    readonly property alias currentWorkspace: workspacesLayer.currentWorkspace
    property alias zoomEnabled: zoomArea.enabled

    id: root
    transform: Scale {
        id: screenScaler
        origin.x: zoomArea.x2
        origin.y: zoomArea.y2
        xScale: zoomArea.zoom2
        yScale: zoomArea.zoom2
    }

    /*
     * Screen zoom handler
     */

    ScreenZoom {
        id: zoomArea
        anchors.fill: parent
        scaler: screenScaler
        enabled: true
        z: 1000
    }

    /*
     * Shell and workspaces
     */

    // Background is below everything
    Item {
        id: backgroundLayer
        anchors.fill: parent
    }

    // Desktop is only above to the background
    Item {
        id: desktopLayer
        anchors.fill: parent
    }

    // Workspaces
    WorkspacesLinearView {
        id: workspacesLayer
        anchors.fill: parent
        screenView: root
    }

    // Panels are above application windows
    Item {
        id: panelsLayer
        anchors.fill: parent
    }

    // Notifications are above panels
    Item {
        id: notificationsLayer
        anchors.fill: parent
    }

    /*
     * Hot corners
     */

    HotCorners {
        id: hotCorners
        anchors.fill: parent
        rotation: 0
        onTopLeftTriggered: workspacesLayer.selectPrevious()
        onTopRightTriggered: workspacesLayer.selectNext()
        onBottomLeftTriggered: compositorRoot.toggleEffect("PresentWindowsGrid")
    }

    /*
     * Important layers
     */

    // Full screen windows can cover application windows and panels
    Item {
        id: fullScreenLayer
        anchors.fill: parent
    }

    // Modal overlay for dialogs
    Rectangle {
        id: modalOverlay
        anchors.fill: parent
        color: "black"
        opacity: 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 250
            }
        }
    }

    // Globally modal dialogs can cover applications and shell gadgets
    Item {
        id: dialogsLayer
        anchors.fill: parent
    }

    // Overlays can cover pretty much everything except the lock screen
    Item {
        id: overlayLayer
        anchors.fill: parent
    }

    // Lock screen is above all windows to shield the session
    Item {
        id: lockLayer
        anchors.fill: parent
    }

    /*
     * Mouse pointer
     */

    // Cursors are above anything
    Item {
        id: cursorLayer
        anchors.fill: parent
    }
}
