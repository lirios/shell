/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects
import Liri.private.shell 1.0 as P
import "../components" as Components
import "../screens"

Item {
    id: screenView

    readonly property alias surfacesArea: desktop.currentWorkspace
    readonly property alias desktop: desktop
    readonly property alias runCommand: runCommand
    readonly property alias authDialog: authDialog

    property bool cursorVisible: true
    property bool locked: false

    property alias showFps: fpsIndicator.visible
    property alias showInformation: outputInfo.visible
    property alias zoomEnabled: zoomArea.enabled

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

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
            PropertyChanges { target: logoutScreen; active: true }
        },
        State {
            name: "poweroff"
            PropertyChanges { target: screenView; cursorVisible: true }
            PropertyChanges { target: powerScreen; active: true }
        },
        State {
            name: "restart"
            PropertyChanges { target: screenView; cursorVisible: true }
            PropertyChanges { target: powerScreen; active: true }
        },
        State {
            name: "lock"
            PropertyChanges { target: screenView; cursorVisible: true }
            PropertyChanges { target: lockScreenLoader; loadComponent: true }
            // FIXME: Before suspend we lock the screen, but turning the output off has a side effect:
            // when the system is resumed it won't flip so we comment this out but unfortunately
            // it means that the lock screen will not turn off the screen
            //StateChangeScript { script: output.idle() }
        }
    ]

    Connections {
        target: SessionInterface
        onSessionLocked: screenView.state = "lock"
        onSessionUnlocked: screenView.state = "session"
        onIdleInhibitRequested: liriCompositor.idleInhibit++
        onIdleUninhibitRequested: liriCompositor.idleInhibit--
    }

    /*
     * Run command dialog
     */

    RunCommand {
        id: runCommand
        x: (parent.width - height) / 2
        y: (parent.height - height) / 2
    }

    /*
     * Hot corners
     */

    // Top-left corner
    Components.HotCorner {
        corner: Qt.TopLeftCorner
        z: 2000
        //onTriggered: workspacesLayer.selectPrevious()
    }

    // Top-right corner
    Components.HotCorner {
        corner: Qt.TopRightCorner
        z: 2000
        //onTriggered: workspacesLayer.selectNext()
    }

    // Bottom-left corner
    Components.HotCorner {
        corner: Qt.BottomLeftCorner
        z: 2000
        onTriggered: {
            if (desktop.currentWorkspace.state == "normal")
                desktop.currentWorkspace.state = "present";
            else
                desktop.currentWorkspace.state = "normal";
        }
    }

    // Bottom-right corner
    Components.HotCorner {
        corner: Qt.BottomRightCorner
        z: 2000
    }

    /*
     * Desktop
     */

    Rectangle {
        anchors.fill: parent
        color: Material.color(Material.Grey, Material.Shade700)

        Desktop {
            id: desktop

            // Margins for "present" mode to fit screen aspect ratio
            property QtObject margins: QtObject {
                property real left: screenView.width * 0.1
                property real right: screenView.width * 0.1
                property real top: screenView.height * 0.1
                property real bottom: screenView.height * 0.1
            }

            anchors.fill: parent

            // All the necessary for the "present" mode
            layer.enabled: false
            layer.effect: FluidEffects.Elevation {
                elevation: 24
            }
            states: [
                State {
                    name: "normal"

                    PropertyChanges {
                        target: desktop
                        anchors.margins: 0
                    }
                },
                State {
                    name: "present"

                    // Margins respect screen aspect ratio
                    PropertyChanges {
                        target: desktop
                        anchors.leftMargin: margins.left
                        anchors.rightMargin: margins.right
                        anchors.topMargin: margins.top
                        anchors.bottomMargin: margins.bottom
                    }
                }

            ]
            transitions: [
                Transition {
                    to: "normal"

                    SequentialAnimation {
                        NumberAnimation {
                            properties: "anchors.leftMargin,anchors.rightMargin,anchors.topMargin,anchors.bottomMargin"
                            easing.type: Easing.OutQuad
                            duration: 300
                        }

                        ScriptAction { script: desktop.layer.enabled = false }
                    }
                },
                Transition {
                    to: "present"

                    SequentialAnimation {
                        ScriptAction { script: desktop.layer.enabled = true }

                        NumberAnimation {
                            properties: "anchors.leftMargin,anchors.rightMargin,anchors.topMargin,anchors.bottomMargin"
                            easing.type: Easing.InQuad
                            duration: 300
                        }
                    }
                }
            ]

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
                duration: FluidControls.Units.longDuration
            }
        }
    }

    /*
     * Authentication
     */

    AuthDialog {
        id: authDialog
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
    }

    /*
     * Lock screen
     */

    Component {
        id: primaryLockScreenComponent

        LockScreen {
            primary: true
        }
    }

    Component {
        id: secondaryLockScreenComponent

        LockScreen {
            primary: false
        }
    }

    FluidControls.Loadable {
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
     * Logout and power off
     */

    LogoutScreen {
        id: logoutScreen

        onCanceled: screenView.state = "session"
    }

    PowerScreen {
        id: powerScreen

        onCanceled: screenView.state = "session"
    }

    /*
     * Full screen indicators
     */

    Text {
        id: fpsIndicator
        anchors {
            top: parent.top
            right: parent.right
        }
        text: fpsCounter.framesPerSecond.toFixed(2)
        font.pointSize: 36
        style: Text.Raised
        styleColor: "#222"
        color: "white"
        z: 1000
        visible: false

        P.FpsCounter {
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

    /*
     * Methods
     */

    function handleKeyPressed(event) {
        // Handle Meta modifier
        if (event.modifiers & Qt.MetaModifier) {
            // Open window switcher
            if (output.primary) {
                if (event.key === Qt.Key_Tab) {
                    event.accepted = true;
                    desktop.windowSwitcher.next();
                    return;
                } else if (event.key === Qt.Key_Backtab) {
                    event.accepted = true;
                    desktop.windowSwitcher.previous();
                    return;
                }
            }
        }

        // Power off and suspend
        switch (event.key) {
        case Qt.Key_PowerOff:
        case Qt.Key_PowerDown:
        case Qt.Key_Suspend:
        case Qt.Key_Hibernate:
            if (screenView.state != "lock")
                screenView.state = "poweroff";
            event.accepted = true;
            return;
        default:
            break;
        }

        event.accepted = false;
    }

    function handleKeyReleased(event) {
        // Handle Meta modifier
        if (event.modifiers & Qt.MetaModifier) {
            // Close window switcher
            if (output.primary) {
                if (event.key === Qt.Key_Super_L || event.key === Qt.Key_Super_R) {
                    event.accepted = true;
                    desktop.windowSwitcher.close();
                    desktop.windowSwitcher.activate();
                    return;
                }
            }
        }

        event.accepted = false;
    }

    function showLogout() {
        if (screenView.state != "lock")
            screenView.state = "logout";
    }

    function showPowerOff() {
        if (screenView.state != "lock")
            screenView.state = "poweroff";
    }

    function showRestart() {
        if (screenView.state != "lock")
            screenView.state = "restart";
    }
}
