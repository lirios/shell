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

Item {
    property string name
    readonly property alias workspacesView: workspacesLayer
    readonly property alias currentWorkspace: workspacesLayer.currentWorkspace

    id: root
    transform: Scale {
        id: scaler
        origin.x: zoomArea.x2
        origin.y: zoomArea.y2
        xScale: zoomArea.zoom2
        yScale: zoomArea.zoom2
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

    // Top-left corner
    HotCorner {
        id: topLeftCorner
        anchors {
            left: parent.left
            top: parent.top
        }
        type: Item.TopLeft
        onTriggered: workspacesLayer.selectPrevious()
    }

    // Top-right corner
    HotCorner {
        id: topRightCorner
        anchors {
            right: parent.right
            top: parent.top
        }
        type: Item.TopRight
        onTriggered: workspacesLayer.selectNext()
    }

    // Bottom-left corner
    HotCorner {
        id: bottomLeftCorner
        anchors {
            left: parent.left
            bottom: parent.bottom
        }
        type: Item.BottomLeft
        onTriggered: compositorRoot.toggleEffect("PresentWindowsGrid")
    }

    // Bottom-right corner
    HotCorner {
        id: bottomRightCorner
        anchors {
            right: parent.right
            bottom: parent.bottom
        }
        type: Item.BottomRight
    }

    /*
     * Zoom controls
     */

    MouseArea {
        property real x1: 0.0
        property real y1: 0.0
        property real x2: 0.0
        property real y2: 0.0
        property real zoom1: 1.0
        property real zoom2: 1.0
        property real min: 1.0
        property real max: 10.0

        id: zoomArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        enabled: false
        hoverEnabled: zoomArea.enabled
        propagateComposedEvents: true
        //drag.target: root
        //drag.filterChildren: true
        onWheel: {
            if (!(wheel.modifiers & Qt.AltModifier)) {
                wheel.accepted = false;
                return;
            }

            zoomArea.x1 = scaler.origin.x;
            zoomArea.y1 = scaler.origin.y;
            zoomArea.zoom1 = scaler.xScale;
            zoomArea.x2 = zoomArea.mouseX;
            zoomArea.y2 = zoomArea.mouseY;

            var newZoom;
            if (wheel.angleDelta.y > 0) {
                newZoom = zoomArea.zoom1 + 0.1;
                if (newZoom <= zoomArea.max)
                    zoomArea.zoom2 = newZoom;
                else
                    zoomArea.zoom2 = zoomArea.max;
            } else {
                newZoom = zoomArea.zoom1 - 0.1;
                if (newZoom >= zoomArea.min)
                    zoomArea.zoom2 = newZoom;
                else
                    zoomArea.zoom2 = zoomArea.min;
            }

            wheel.accepted = true;
        }
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
        z: 8
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
