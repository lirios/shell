/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtCompositor 1.0
import GreenIsland 1.0
import GreenIsland.Core 1.0
import "WindowManagement.js" as WindowManagement
import "."

Item {
    property alias layers: layers

    id: root

    Timer {
        id: idleTimer
        interval: compositor.idleInterval
        onIntervalChanged: {
            if (running)
                restart();
        }
    }

    ListModel {
        id: surfaceModel
    }

    Connections {
        target: compositor
        onIdleInhibitResetRequested: compositor.idleInhibit = 0
        onIdleTimerStartRequested: idleTimer.running = true
        onIdleTimerStopRequested: idleTimer.running = false
        onIdle: {
            // Fade the desktop out
            splash.opacity = 1.0;

            // Lock the session
            compositor.lockSession();
        }
        onWake: {
            // Unlock the session
            compositor.unlockSession();
        }
        onFadeIn: {
            // Fade the desktop in
            splash.opacity = 0.0;

            // Damage all surfaces
            //damageAll();
        }
        onFadeOut: {
            // Fade the desktop out
            splash.opacity = 1.0;
        }
        onUnlocked: {
            // Fade the desktop in
            splash.opacity = 0.0;

            // Damage all surfaces
            //damageAll();
        }
        onReady: {
            // Fade the desktop in
            splash.opacity = 0.0;

            // Start idle timer
            idleTimer.running = true
        }
        onWorkspaceAdded: {
            // Add a new workspace
            Workspaces.addWorkspace();
        }
        onWorkspaceRemoved: {
            // Remove workspace
            Workspaces.removeWorkspace(index);
        }
        onWorkspaceSelected: {
            // Select workspace
            Workspaces.selectWorkspace(index);
        }
        onSurfaceMapped: {
            // A surface was mapped
            WindowManagement.surfaceMapped(surface);
        }
        onSurfaceUnmapped: {
            // A surface was unmapped
            WindowManagement.surfaceUnmapped(surface);
        }
        onSurfaceDestroyed: {
            // A surface was destroyed
            WindowManagement.surfaceDestroyed(surface);
        }
    }

    /*
     * Components
     */

    Rectangle {
        color: "red"
        x: 10
        y: 10
        width: 50
        height: 50
        z: 900000

        Text {
            anchors.fill: parent
            text: "<-- (" + Workspaces.currentIndex + ")"
        }

        MouseArea {
            anchors.fill: parent
            preventStealing: true
            onClicked: {
                Workspaces.selectPreviousWorkspace();
            }
        }
    }

    Rectangle {
        color: "red"
        x: 80
        y: 10
        width: 50
        height: 50
        z: 900000

        Text {
            anchors.fill: parent
            text: "--> (" + Workspaces.currentIndex + ")"
        }

        MouseArea {
            anchors.fill: parent
            preventStealing: true
            onClicked: {
                Workspaces.selectNextWorkspace();
            }
        }
    }

    // FPS counter
    Text {
        anchors {
            top: parent.top
            right: parent.right
        }
        z: 1000
        text: fpsCounter.fps
        font.pointSize: 36
        style: Text.Raised
        styleColor: "#222"
        color: "white"
        visible: false

        FpsCounter {
            id: fpsCounter
        }
    }

    // Black rectangle for fade-in and fade-out effects
    Rectangle {
        id: splash
        anchors.fill: parent
        color: "black"
        z: 999

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 250
            }
        }
    }

    // Layers for windows
    Layers {
        id: layers
        anchors.fill: parent
        z: 998
    }

    Component.onCompleted: Workspaces.workspacesView = layers.workspaces
}
