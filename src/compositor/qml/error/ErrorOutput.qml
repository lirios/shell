// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtWayland.Compositor 1.15
import Liri.WaylandServer 1.0 as WS
import Liri.private.shell 1.0 as P

P.WaylandOutput {
    id: output

    readonly property bool primary: liriCompositor.defaultOutput === this

    property var screen: null

    Component.onCompleted: {
        if (output.screen) {
            for (var i = 0; i < output.screen.modes.length; i++) {
                var screenMode = output.screen.modes[i];
                var isPreferred = output.screen.preferredMode === screenMode;
                var isCurrent = output.screen.currentMode === screenMode;
                output.addOutputMode(screenMode.resolution, screenMode.refreshRate, isPreferred, isCurrent);
            }
        }
    }

    window: Window {
        id: outputWindow

        x: output.position.x
        y: output.position.y
        width: output.geometry.width
        height: output.geometry.height
        flags: Qt.Window | Qt.FramelessWindowHint
        screen: output.screen ? Qt.application.screens[output.screen.screenIndex] : null
        color: "black"
        visible: true

        // Grab surface from shell helper
        WaylandQuickItem {
            id: grabItem
            focusOnClick: false
            onSurfaceChanged: {
                shellHelper.grabCursor(WS.LiriShell.ArrowGrabCursor);
                if (output.primary)
                    grabItem.setPrimary();
            }
        }

        WaylandMouseTracker {
            id: mouseTracker
            anchors.fill: parent
            windowSystemCursorEnabled: true

            ErrorScreenView {
                id: screenView
                anchors.fill: parent
            }

            WaylandCursorItem {
                id: cursor
                seat: output.compositor.defaultSeat
                x: mouseTracker.mouseX - hotspotX
                y: mouseTracker.mouseY - hotspotY
                visible: mouseTracker.containsMouse && !mouseTracker.windowSystemCursorEnabled
            }
        }
    }
}
