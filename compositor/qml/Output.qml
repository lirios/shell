/****************************************************************************
 * This file is part of Liri.
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

import QtQuick 2.5
import QtQuick.Controls 2.0
import QtWayland.Compositor 1.0
import Liri.WaylandServer 1.0
import Liri.Shell 1.0
import "desktop"

ExtendedOutput {
    id: output

    readonly property bool primary: compositor.defaultOutput === this

    property var viewsBySurface: ({})
    property var grabItem: null

    property int idleInhibit: 0

    readonly property alias screenView: screenView
    readonly property Item surfacesArea: screenView.surfacesArea
    //readonly property alias idleDimmer: idleDimmer
    readonly property alias cursor: cursor

    manufacturer: nativeScreen.manufacturer
    model: nativeScreen.model
    position: nativeScreen.position
    physicalSize: nativeScreen.physicalSize
    subpixel: nativeScreen.subpixel
    transform: nativeScreen.transform
    scaleFactor: nativeScreen.scaleFactor
    sizeFollowsWindow: false
    automaticFrameCallback: powerState === ExtendedOutput.PowerStateOn

    window: ApplicationWindow {
        id: window

        x: nativeScreen.position.x
        y: nativeScreen.position.y
        width: nativeScreen.size.width
        height: nativeScreen.size.height
        flags: Qt.FramelessWindowHint

        // Virtual Keyboard
        Loader {
            parent: window.overlay
            source: Qt.resolvedUrl("base/Keyboard.qml")
            x: (parent.width - width) / 2
            y: parent.height - height
            z: 999
            width: Math.max(parent.width / 2, 768)
        }

        // Keyboard handling
        KeyEventFilter {
            Keys.onPressed: {
                // Input wakes the output
                compositor.wake();

                screenView.handleKeyPressed(event);
            }

            Keys.onReleased: {
                // Input wakes the output
                compositor.wake();

                screenView.handleKeyReleased(event);
            }
        }

        // Mouse tracker
        WindowMouseTracker {
            id: mouseTracker

            anchors.fill: parent

            windowSystemCursorEnabled: false

            onMouseXChanged: {
                // Wake up
                compositor.wake();

                // Update global mouse position
                compositor.mousePos.x = output.position.x + mouseX;
            }
            onMouseYChanged: {
                // Wake up
                compositor.wake();

                // Update global mouse position
                compositor.mousePos.y = output.position.y + mouseY;
            }
            // TODO: Need to wake up with mouse button pressed, released and wheel

            // User interface
            ScreenView {
                id: screenView
                anchors.fill: parent
            }

            // Idle dimmer
            IdleDimmer {
                id: idleDimmer

                anchors.fill: parent

                output: output

                z: 1000002
            }
        }

        // Pointer cursor
        WaylandCursorItem {
            id: cursor

            parent: mouseTracker.parent
            seat: output.compositor.defaultSeat

            x: mouseTracker.mouseX
            y: mouseTracker.mouseY
            z: 1000001

            visible: mouseTracker.containsMouse &&
                     screenView.cursorVisible &&
                     output.powerState === ExtendedOutput.PowerStateOn
        }
    }

    QtObject {
        id: __private

        property bool idle: false
    }

    /*
     * Methods
     */

    function wake() {
        if (!__private.idle)
            return;

        console.debug("Power on output", manufacturer, model);
        idleDimmer.fadeOut();
        output.powerState = ExtendedOutput.PowerStateOn;
        __private.idle = false;
    }

    function idle() {
        if (__private.idle)
            return;

        console.debug("Standby output", manufacturer, model);
        idleDimmer.fadeIn();
        __private.idle = true;
    }

    function activateShellSurface(shellSurface) {
        for (var surface in viewsBySurface) {
            if (shellSurface.surface === surface) {
                viewsBySurface[surface].activate();
                break;
            }
        }
    }
}
