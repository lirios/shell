/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import GreenIsland 1.0 as GreenIsland
import Fluid.Controls 1.0
import "../base"
import "../screens"
import ".."

BaseScreenView {
    id: screenView

    readonly property alias surfacesArea: desktop.currentWorkspace
    readonly property alias desktop: desktop
    readonly property alias runCommand: runCommand

    property alias showFps: fpsIndicator.visible
    property alias showInformation: outputInfo.visible
    property alias zoomEnabled: zoomArea.enabled

    state: "splash"
    states: [
        State {
            name: "splash"
            PropertyChanges { target: screenView; cursorVisible: false }
            PropertyChanges { target: splashScreen; opacity: 1.0 }
        },
        State {
            name: "session"
            PropertyChanges { target: screenView; cursorVisible: true }
        },
        State {
            name: "logout"
            PropertyChanges { target: screenView; cursorVisible: true }
            PropertyChanges { target: logoutLoader; loadComponent: true; mode: "logout" }
        },
        State {
            name: "poweroff"
            PropertyChanges { target: screenView; cursorVisible: true }
            PropertyChanges { target: logoutLoader; loadComponent: true; mode: "poweroff" }
        },
        State {
            name: "restart"
            PropertyChanges { target: screenView; cursorVisible: true }
            PropertyChanges { target: logoutLoader; loadComponent: true; mode: "restart" }
        },
        State {
            name: "lock"
            PropertyChanges { target: screenView; cursorVisible: true }
            PropertyChanges { target: logoutLoader; loadComponent: false }
            PropertyChanges { target: lockScreenLoader; loadComponent: true }
            // FIXME: Before suspend we lock the screen, but turning the output off has a side effect:
            // when the system is resumed it won't flip so we comment this out but unfortunately
            // it means that the lock screen will not turn off the screen
            //StateChangeScript { script: output.idle() }
        },
        State {
            name: "shield"
            PropertyChanges { target: screenView; cursorVisible: true }
            PropertyChanges { target: shieldLoader; source: "Shield.qml"; visible: true }
        }
    ]

    onKeyPressed: {
        // Handle Meta modifier
        if (event.modifiers & Qt.MetaModifier) {
            // Open window switcher
            if (output.primary) {
                if (event.key === Qt.Key_Tab) {
                    event.accept = true;
                    screenView.windowSwitcher.next();
                    return;
                } else if (event.key === Qt.Key_Backtab) {
                    event.accept = true;
                    screenView.windowSwitcher.previous();
                    return;
                }
            }
        }

        event.accept = false;
    }

    onKeyReleased: {
        // Handle Meta modifier
        if (event.modifiers & Qt.MetaModifier) {
            // Close window switcher
            if (output.primary) {
                if (event.key === Qt.Key_Super_L || event.key === Qt.Key_Super_R) {
                    event.accept = true;
                    desktop.windowSwitcher.close();
                    desktop.windowSwitcher.activate();
                    return;
                }
            }
        }

        event.accept = false;
    }

    Connections {
        target: SessionInterface
        onSessionLocked: screenView.state = "lock"
        onSessionUnlocked: screenView.state = "session"
        onIdleInhibitRequested: compositor.idleInhibit++
        onIdleUninhibitRequested: compositor.idleInhibit--
        onShutdownRequestCanceled: screenView.state = "session"
        onLogOutRequested: if (screenView.state != "lock") screenView.state = "logout"
        onPowerOffRequested: if (screenView.state != "lock") screenView.state = "poweroff"
        onRestartRequested: if (screenView.state != "lock") screenView.state = "restart"
    }

    /*
     * Run command dialog
     */

    RunCommand {
        id: runCommand
        x: (parent.width - height) / 2
        y: (parent.height - height) / 2
    }

    Desktop {
        id: desktop

        anchors.fill: parent

        transform: Scale {
            id: screenScaler
            origin.x: zoomArea.x2
            origin.y: zoomArea.y2
            xScale: zoomArea.zoom2
            yScale: zoomArea.zoom2
        }


        ScreenZoom {
            id: zoomArea
            anchors.fill: parent
            scaler: screenScaler
            enabled: false
        }

        Component.onCompleted: screenView.state = "session"
    }

    /*
     * Splash screen
     */

    Loader {
        id: splashScreen
        anchors.fill: parent
        source: "../screens/SplashScreen.qml"
        opacity: 0.0
        active: false
        z: 900
        onOpacityChanged: {
            if (opacity == 1.0)
                splashScreen.active = true;
            else if (opacity == 0.0)
                splashScreenTimer.start();
        }

        // Unload after a while so that the opacity animation is visible
        Timer {
            id: splashScreenTimer
            running: false
            interval: 5000
            onTriggered: splashScreen.active = false
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: Units.longDuration
            }
        }
    }

    /*
     * Lock screen
     */

    Component {
        id: primaryLockScreenComponent

        LockScreen {}
    }

    Component {
        id: secondaryLockScreenComponent

        SecondaryLockScreen {}
    }

    Loadable {
        property bool loadComponent: false

        id: lockScreenLoader
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        asynchronous: true
        component: output.primary ? primaryLockScreenComponent : secondaryLockScreenComponent
        z: 900
        onLoadComponentChanged: if (loadComponent) show(); else hide();
    }

    /*
     * Logout screen
     */

    Loadable {
        property bool loadComponent: false
        // FIXME: mode should be empty by default and the LogoutScreen
        // component should handle an empty state, hiding the controls
        property string mode: "logout"

        id: logoutLoader
        anchors.fill: parent
        asynchronous: true
        component: Component {
            LogoutScreen {
                mode: logoutLoader.mode
            }
        }
        z: 900
        onLoadComponentChanged: if (loadComponent) show(); else hide();
    }

    Connections {
        target: logoutLoader.item
        onSuspendRequested: mainItem.state = "lock"
        onCancel: SessionInterface.cancelShutdownRequest()
    }

    /*
     * Screen view
     */



    /*
     * Full screen indicators
     */

    Text {
        id: fpsIndicator
        anchors {
            top: parent.top
            right: parent.right
        }
        text: fpsCounter.fps
        font.pointSize: 36
        style: Text.Raised
        styleColor: "#222"
        color: "white"
        z: 1000
        visible: false

        GreenIsland.FpsCounter {
            id: fpsCounter
        }
    }

    OutputInfo {
        id: outputInfo
        anchors {
            left: parent.left
            top: parent.top
        }
        z: 1000
        visible: false
    }
}
