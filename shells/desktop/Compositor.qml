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
import org.hawaii.misc 0.1
import org.kde.plasma.core 2.0 as PlasmaCore
import "WindowManagement.js" as WindowManagement
import "screen"

Item {
    readonly property alias screenView: screenView
    readonly property alias surfaceModel: surfaceModel
    readonly property int activeWindowIndex: WindowManagement.getActiveWindowIndex()

    signal keyPressed(var event)
    signal keyReleased(var event)

    id: compositorRoot
    state: "session"
    states: [
        State {
            name: "session"
            PropertyChanges { target: keyFilter; enabled: true }
            PropertyChanges { target: shieldLoader; source: ""; visible: false }
            PropertyChanges { target: logoutLoader; source: ""; visible: false }
            PropertyChanges { target: lockScreenLoader; source: ""; z: 899 }
            StateChangeScript { script: enableInput() }
        },
        State {
            name: "logout"
            PropertyChanges { target: keyFilter; enabled: false }
            PropertyChanges { target: shieldLoader; source: "Shield.qml"; visible: true }
            PropertyChanges { target: logoutLoader; source: "LogoutScreen.qml"; visible: true }
        },
        State {
            name: "lock"
            PropertyChanges { target: keyFilter; enabled: false }
            PropertyChanges { target: lockScreenLoader; source: "LockScreen.qml"; z: 910 }
            StateChangeScript { script: disableInput() }
        },
        State {
            name: "shield"
            PropertyChanges { target: shieldLoader; source: "Shield.qml"; visible: true }
        }
    ]
    onKeyPressed: {
        // Abort session
        // TODO: Handle this as a keybinding
        if (event.modifiers & (Qt.ControlModifier | Qt.AltModifier) && event.key === Qt.Key_Backspace) {
            event.accepted = true;
            compositor.abortSession();
        }

        // Lock screen
        if (event.modifiers & Qt.MetaModifier && event.key === Qt.Key_L) {
            state = "lock";
            event.accepted = true;
        }
    }

    Timer {
        id: idleTimer
        //interval: compositor.idleInterval
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
            screenView.layers.splash.opacity = 1.0;

            // Lock the session
            compositor.lockSession();
        }
        onWake: {
            // Unlock the session
            compositor.unlockSession();
        }
        onFadeIn: {
            // Fade the desktop in
            screenView.layers.splash.opacity = 0.0;

            // Bring user layer up
            screenView.state = "user";
        }
        onFadeOut: {
            // Bring splash layer up
            screenView.state = "splash";

            // Fade the desktop out
            screenView.layers.splash.opacity = 1.0;
        }
        onLocked: {
            // Bring lock layer up
            screenView.state = "lock";
        }
        onUnlocked: {
            // Fade the desktop in
            screenView.layers.splash.opacity = 0.0;

            // Bring user layer up
            screenView.state = "user";
        }
        onReady: {
            // Fade the desktop in
            screenView.layers.splash.opacity = 0.0;

            // Bring user layer up
            screenView.state = "user";

            // Start idle timer
            idleTimer.running = true
        }
        onWorkspaceAdded: {
            // Add a new workspace
            screenView.workspacesView.add();
        }
        onWorkspaceRemoved: {
            // Remove workspace
            screenView.workspacesView.remove(index);
        }
        onWorkspaceSelected: {
            // Select workspace
            screenView.workspacesView.select(index);
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

    // Key events filter
    KeyEventFilter {
        id: keyFilter

        Keys.onPressed: compositorRoot.keyPressed(event)
        Keys.onReleased: compositorRoot.keyReleased(event)
    }

    // Screen
    ScreenView {
        id: screenView
        anchors.fill: parent
        z: 900
    }

    // Shield
    Loader {
        id: shieldLoader
        anchors.fill: parent
        asynchronous: true
        z: progress > 0 ? 901 : 899
        opacity: progress

        Behavior on z {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: units.longDuration
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: units.longDuration
            }
        }
    }

    /*
     * Logout screen
     */

    Loader {
        id: logoutLoader
        anchors.fill: parent
        asynchronous: true
        z: progress > 0 ? 910 : 899
        opacity: progress

        Behavior on z {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: units.longDuration
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: units.longDuration
            }
        }
    }

    Connections {
        target: logoutLoader.item
        onCancel: compositorRoot.state = "session"
    }

    /*
     * Lock screen
     */

    Loader {
        id: lockScreenLoader
        anchors.fill: parent
        asynchronous: true
        z: 899

        Behavior on z {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: units.longDuration
            }
        }
    }

    Connections {
        target: lockScreenLoader.item
        onUnlocked: compositorRoot.state = "session"
    }

    /*
     * Methods
     */

    function toggleEffect(name) {
        screenView.workspacesView.currentWorkspace.effects.toggle(name);
    }

    function moveFront(window) {
        return WindowManagement.moveFront(window);
    }

    function enableInput() {
        var i;
        for (i = 0; i < compositorRoot.surfaceModel.count; i++) {
            var window = compositorRoot.surfaceModel.get(i).window;
            window.child.focus = true;
        }
    }

    function disableInput() {
        var i;
        for (i = 0; i < compositorRoot.surfaceModel.count; i++) {
            var window = compositorRoot.surfaceModel.get(i).window;
            window.child.focus = false;
        }
    }
}
