// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import Aurora.Compositor
import Liri.Shell.Compositor as LS

LS.WaylandOutput {
    id: output

    readonly property bool primary: liriCompositor.defaultOutput === this

    window: Window {
        id: outputWindow

        x: output.position.x
        y: output.position.y
        width: output.geometry.width
        height: output.geometry.height
        flags: Qt.Window | Qt.FramelessWindowHint
        screen: output.platformOutput?.screen || null
        color: "black"
        visible: output.platformOutput && output.platformOutput.enabled

        WaylandMouseTracker {
            id: mouseTracker

            anchors.fill: parent
            windowSystemCursorEnabled: containsMouse

            ErrorScreenView {
                id: screenView
                anchors.fill: parent
            }

            LS.WaylandCursorGrabber {
                seat: output.compositor.defaultSeat
                grab: mouseTracker.containsMouse
            }
        }
    }
}
