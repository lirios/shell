/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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
import QtQuick.Controls 2.0
import GreenIsland 1.0 as GreenIsland
import Hawaii.Shell 1.0
import "../desktop"

ApplicationWindow {
    id: window

    property var output

    property alias screenView: screenViewLoader.item
    property alias idleDimmer: idleDimmer
    property alias cursor: cursor

    property alias screenViewComponent: screenViewLoader.sourceComponent

    readonly property alias grabItem: grabItem

    x: nativeScreen.position.x
    y: nativeScreen.position.y
    width: nativeScreen.size.width
    height: nativeScreen.size.height
    flags: Qt.FramelessWindowHint

    Binding {
        target: screenView
        property: "output"
        value: window.output
    }

    // Grab surface from shell helper
    GreenIsland.WaylandQuickItem {
        id: grabItem
        anchors.fill: parent
        focusOnClick: false
        onSurfaceChanged: compositor.shellHelper.grabCursor(ShellHelper.ArrowGrabCursor)
    }

    // Virtual Keyboard
    Loader {
        parent: window.overlay
        source: Qt.resolvedUrl("Keyboard.qml")
        x: (parent.width - width) / 2
        y: parent.height - height
        z: 999
        width: Math.max(parent.width / 2, 768)
    }

    // Keyboard handling
    GreenIsland.KeyEventFilter {
        Keys.onPressed: {
            // Input wakes the output
            compositor.wake()

            screenView.keyPressed(event)
        }

        Keys.onReleased: {
            // Input wakes the output
            compositor.wake()

            screenView.keyReleased(event)
        }
    }

    GreenIsland.WaylandMouseTracker {
        id: localPointerTracker

        anchors.fill: parent
        windowSystemCursorEnabled: false

        onMouseXChanged: compositor.wake()
        onMouseYChanged: compositor.wake()
        // TODO: Need to wake up with mouse button pressed, released and wheel

        Loader {
            id: screenViewLoader
            anchors.fill: parent
        }

        GreenIsland.WaylandCursorItem {
            id: cursor
            seat: output.compositor.defaultSeat
            x: localPointerTracker.mouseX - hotspotX
            y: localPointerTracker.mouseY - hotspotY
            visible: localPointerTracker.containsMouse &&
                     screenView.cursorVisible &&
                     output.powerState === GreenIsland.ExtendedOutput.PowerStateOn
        }
    }

    // Idle dimmer
    IdleDimmer {
        id: idleDimmer
        anchors.fill: parent
        output: window.output
    }
}
