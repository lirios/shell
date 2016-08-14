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

ApplicationWindow {
    id: window

    property var output

    property alias screenView: screenViewLoader.item
    property alias idleDimmer: idleLoader.item
    property alias cursor: cursor

    property alias screenViewComponent: screenViewLoader.sourceComponent
    property alias idleDimmerComponent: idleLoader.sourceComponent

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

    // Idle dimmer
    Loader {
        id: idleLoader
        anchors.fill: parent
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
        enableWSCursor: true

        onMouseXChanged: compositor.wake()
        onMouseYChanged: compositor.wake()

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
                     output.powerState === GreenIsland.ExtendedOutput.PowerStateOn
        }
    }
}
