// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import Aurora.Compositor 1.0
import Liri.Shell.Compositor 1.0 as LS

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
        screen: output.screen ? Qt.application.screens[output.screen.screenIndex] : null
        color: "black"
        visible: true

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
