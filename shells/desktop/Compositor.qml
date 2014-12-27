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
    readonly property var windowList: WindowManagement.windowList

    signal keyPressed(var event)
    signal keyReleased(var event)

    signal windowSwitchPrev()
    signal windowSwitchNext()
    signal windowSwitchSelect()

    id: compositorRoot
    state: "session"
    states: [
        State {
            name: "session"
            PropertyChanges { target: keyFilter; enabled: true }
            PropertyChanges { target: windowSwitcherLoader; source: ""; z: 899 }
            PropertyChanges { target: shieldLoader; source: ""; visible: false }
            PropertyChanges { target: logoutLoader; source: ""; z: 899 }
            PropertyChanges { target: lockScreenLoader; source: ""; z: 899 }
            PropertyChanges { target: splashScreen; opacity: 0.0 }
            StateChangeScript { script: enableInput() }
        },
        State {
            name: "splash"
            PropertyChanges { target: splashScreen; opacity: 1.0 }
        },
        State {
            name: "windowSwitcher"
            PropertyChanges { target: windowSwitcherLoader; source: "WindowSwitcher.qml"; z: 910 }
            StateChangeScript { script: disableInput() }
        },
        State {
            name: "logout"
            PropertyChanges { target: keyFilter; enabled: false }
            PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
            PropertyChanges { target: logoutLoader; source: "LogoutScreen.qml"; z: 910 }
            StateChangeScript { script: disableInput() }
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
            return;
        }

        // Lock screen
        if (event.modifiers & Qt.MetaModifier && event.key === Qt.Key_L) {
            state = "lock";
            event.accepted = true;
            return;
        }

        // Window switcher
        if (event.modifiers & Qt.MetaModifier) {
            if (event.key === Qt.Key_Tab || event.key === Qt.Key_Backtab) {
                if (state != "windowSwitcher" && surfaceModel.count >= 2) {
                    // Activate only when two or more windows are available
                    state = "windowSwitcher";
                    event.accepted = true;
                    return;
                }
            }
        }
    }
    onKeyReleased: {
        // Window switcher
        if (state == "windowSwitcher") {
            if (event.modifiers & Qt.MetaModifier) {
                // Cycle between windows
                if (event.key === Qt.Key_Tab)
                    compositorRoot.windowSwitchNext();
                else if (event.key === Qt.Key_Backtab)
                    compositorRoot.windowSwitchPrev();
            } else {
                // Give focus to the selected window
                compositorRoot.windowSwitchSelect();

                // Keys released, deactivate switcher
                state = "session";
            }

            event.accepted = true;
            return;
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
            compositorRoot.state = "splash";

            // Lock the session
            compositor.lockSession();
        }
        onWake: {
            // Unlock the session
            compositor.unlockSession();
        }
        onFadeIn: {
            // Bring user layer up
            compositorRoot.state = "session";
        }
        onFadeOut: {
            // Fade the desktop out
            compositorRoot.state = "splash";
        }
        onLocked: {
            // Bring lock layer up
            compositorRoot.state = "lock";
        }
        onUnlocked: {
            // Bring user layer up
            compositorRoot.state = "session";
        }
        onReady: {
            // Bring user layer up
            compositorRoot.state = "session";

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

    /*
     * Splash
     */

    Rectangle {
        id: splashScreen
        anchors.fill: parent
        color: "black"
        z: opacity == 0.0 ? 899 : 910
        opacity: 0.0

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
     * Window switcher
     */

    Loader {
        id: windowSwitcherLoader
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        asynchronous: true
        z: 899
        width: parent.width - (units.largeSpacing * 2)
        height: (parent.height * 0.5) - (units.largeSpacing * 2)

        Behavior on z {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: units.longDuration
            }
        }
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
        z: 899

        Behavior on z {
            NumberAnimation {
                easing.type: Easing.InOutQuad
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
