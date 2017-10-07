/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.5
import QtQuick.Window 2.3
import QtQuick.Controls 2.0
import QtWayland.Compositor 1.0
import Liri.WaylandServer 1.0 as LiriWayland
import Liri.Shell 1.0 as LiriShell
import "desktop"

LiriWayland.WaylandOutput {
    id: output

    property bool primary: false

    property alias screen: outputSettings.screen
    property alias windowScreen: outputWindow.screen
    property alias powerState: outputSettings.powerState

    property var viewsBySurface: ({})

    property int idleInhibit: 0

    readonly property alias screenView: screenView
    readonly property Item surfacesArea: screenView.surfacesArea
    //readonly property alias idleDimmer: idleDimmer
    readonly property alias cursor: cursor

    property bool __idle: false

    automaticFrameCallback: outputSettings.powerState === LiriWayland.WaylandOutputSettings.PowerStateOn

    onPrimaryChanged: {
        // Set default output
        if (primary)
            liriCompositor.defaultOutput = this;
    }

    window: ApplicationWindow {
        id: outputWindow

        x: output.position.x
        y: output.position.y
        width: output.geometry.width
        height: output.geometry.height
        flags: Qt.FramelessWindowHint
        visible: true

        Component.onCompleted: {
            // Register this window with the screencaster
            screencaster.addWindow(this);
        }

        // Virtual Keyboard
        Loader {
            parent: outputWindow.overlay
            active: liriCompositor.settings.ui.inputMethod === "qtvirtualkeyboard"
            source: Qt.resolvedUrl("base/Keyboard.qml")
            x: (parent.width - width) / 2
            y: parent.height - height
            z: 999
            width: Math.max(parent.width / 2, 768)
        }

        // Keyboard handling
        LiriWayland.KeyEventFilter {
            Keys.onPressed: {
                // Input wakes the output
                liriCompositor.wake();

                screenView.handleKeyPressed(event);
            }

            Keys.onReleased: {
                // Input wakes the output
                liriCompositor.wake();

                screenView.handleKeyReleased(event);
            }
        }

        // Mouse tracker
        LiriShell.WindowMouseTracker {
            id: mouseTracker

            anchors.fill: parent

            windowSystemCursorEnabled: platformName !== "liri"

            onMouseXChanged: {
                // Wake up
                liriCompositor.wake();

                // Update global mouse position
                liriCompositor.mousePos.x = output.position.x + mouseX;
            }
            onMouseYChanged: {
                // Wake up
                liriCompositor.wake();

                // Update global mouse position
                liriCompositor.mousePos.y = output.position.y + mouseY;
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
            seat: liriCompositor.defaultSeat

            x: mouseTracker.mouseX
            y: mouseTracker.mouseY
            z: 1000001

            visible: mouseTracker.containsMouse &&
                     !mouseTracker.windowSystemCursorEnabled &&
                     screenView.cursorVisible
        }
    }

    LiriWayland.WaylandOutputSettings {
        id: outputSettings
    }

    /*
     * Methods
     */

    function wake() {
        if (!__idle)
            return;

        console.debug("Power on output", manufacturer, model);
        idleDimmer.fadeOut();
        outputSettings.powerState = LiriWayland.WaylandOutputSettings.PowerStateOn;
        __idle = false;
    }

    function idle() {
        if (__idle)
            return;

        console.debug("Standby output", manufacturer, model);
        idleDimmer.fadeIn();
        __idle = true;
    }
}
