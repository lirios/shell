/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import Qt.labs.controls.material 1.0 as LabsMaterial
import GreenIsland 1.0 as GreenIsland
import Fluid.Ui 1.0 as FluidUi
import "../screens"
import ".."

GreenIsland.WaylandOutput {
    readonly property bool primary: hawaiiCompositor.primaryScreen === nativeScreen

    property int idleInhibit: 0

    readonly property alias surfacesArea: screenView.currentWorkspace
    readonly property alias screenView: screenView
    readonly property alias runCommand: runCommand

    property QtObject activeWindow: null

    property alias showFps: fpsIndicator.visible
    property alias showInformation: outputInfo.visible
    property alias zoomEnabled: zoomArea.enabled

    id: output
    manufacturer: nativeScreen.manufacturer
    model: nativeScreen.model
    position: nativeScreen.position
    currentModeId: nativeScreen.currentMode
    physicalSize: nativeScreen.physicalSize
    subpixel: nativeScreen.subpixel
    transform: nativeScreen.transform
    scaleFactor: nativeScreen.scaleFactor
    sizeFollowsWindow: false
    automaticFrameCallback: powerState === GreenIsland.WaylandOutput.PowerStateOn
    window: ApplicationWindow {
        id: window
        minimumWidth: 1024
        minimumHeight: 768
        maximumWidth: nativeScreen.width
        maximumHeight: nativeScreen.height

        LabsMaterial.Material.theme: LabsMaterial.Material.Dark
        LabsMaterial.Material.primary: LabsMaterial.Material.Blue
        LabsMaterial.Material.accent: LabsMaterial.Material.LightBlue

        Connections {
            target: SessionInterface
            onSessionLocked: mainItem.state = "lock"
            onSessionUnlocked: mainItem.state = "session"
            onIdleInhibitRequested: hawaiiCompositor.idleInhibit++
            onIdleUninhibitRequested: hawaiiCompositor.idleInhibit--
            onShutdownRequestCanceled: mainItem.state = "session"
            onLogOutRequested: mainItem.state = "logout"
            onPowerOffRequested: mainItem.state = "poweroff"
            onRestartRequested: mainItem.state = "restart"
        }

        /*
         * Idle manager
         */

        Rectangle {
            id: blackRect
            parent: window.overlay
            anchors.fill: parent
            color: "black"
            opacity: 0.0
            z: 1000
            onOpacityChanged: {
                if (opacity == 1.0)
                    output.powerState = GreenIsland.WaylandOutput.PowerStateStandby;
            }

            OpacityAnimator {
                id: blackRectAnimator
                target: blackRect
                easing.type: Easing.OutSine
                duration: FluidUi.Units.longDuration
            }

            Timer {
                id: blackRectTimer
                interval: 1000
                onTriggered: {
                    blackRectAnimator.from = 1.0;
                    blackRectAnimator.to = 0.0;
                    blackRectAnimator.start();
                }
            }

            function fadeIn() {
                if (blackRect.opacity == 1.0)
                    return;
                blackRectAnimator.from = 0.0;
                blackRectAnimator.to = 1.0;
                blackRectAnimator.start();
            }

            function fadeOut() {
                if (blackRect.opacity == 0.0)
                    return;
                // Use a timer to compensate for power on time
                blackRectTimer.start();
            }
        }

        /*
         * Virtual keyboard
         */

        Loader {
            parent: window.overlay
            source: "Keyboard.qml"
            x: (parent.width - width) / 2
            y: parent.height - height
            z: 999
            width: Math.max(parent.width / 2, FluidUi.Units.dp(768))
        }

        /*
         * Keyboard handler
         */

        GreenIsland.KeyEventFilter {
            Keys.onPressed: {
                // Input wakes the output
                hawaiiCompositor.wake();

                // Handle Meta modifier
                if (event.modifiers & Qt.MetaModifier) {
                    // Open window switcher
                    if (output.primary) {
                        if (event.key === Qt.Key_Tab) {
                            screenView.windowSwitcher.next();
                            event.accept = true;
                            return;
                        } else if (event.key === Qt.Key_Backtab) {
                            screenView.windowSwitcher.previous();
                            event.accept = true;
                            return;
                        }
                    }
                }

                event.accept = false;
            }

            Keys.onReleased: {
                // Input wakes the output
                hawaiiCompositor.wake();

                // Handle Meta modifier
                if (event.modifiers & Qt.MetaModifier) {
                    // Close window switcher
                    if (output.primary) {
                        if (event.key === Qt.Key_Super_L || event.key === Qt.Key_Super_R) {
                            screenView.windowSwitcher.close();
                            screenView.windowSwitcher.activate();
                            event.accept = true;
                            return;
                        }
                    }
                }

                event.accept = false;
            }
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
         * Contents
         */

        GreenIsland.LocalPointerTracker {
            id: localPointerTracker
            anchors.fill: parent
            globalTracker: globalPointerTracker
            onMouseXChanged: {
                // Input wakes the output
                hawaiiCompositor.wake();
            }
            onMouseYChanged: {
                // Input wakes the output
                hawaiiCompositor.wake();
            }

            Item {
                id: mainItem
                anchors.fill: parent
                state: "splash"
                states: [
                    State {
                        name: "splash"
                        PropertyChanges { target: cursor; visible: false }
                        PropertyChanges { target: splashScreen; opacity: 1.0 }
                    },
                    State {
                        name: "session"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: splashScreen; opacity: 0.0 }
                        //PropertyChanges { target: screenView.windowsSwitcher; active: false }
                        PropertyChanges { target: splashScreen; opacity: 0.0 }
                    },
                    State {
                        name: "logout"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: logoutLoader; loadComponent: true; mode: "logout" }
                    },
                    State {
                        name: "poweroff"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: logoutLoader; loadComponent: true; mode: "poweroff" }
                    },
                    State {
                        name: "restart"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: logoutLoader; loadComponent: true; mode: "restart" }
                    },
                    State {
                        name: "lock"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: screenView.windowsSwitcherLoader; active: false }
                        PropertyChanges { target: logoutLoader; loadComponent: false }
                        PropertyChanges { target: lockScreenLoader; loadComponent: true }
                        StateChangeScript { script: output.idle() }
                    },
                    State {
                        name: "shield"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: shieldLoader; source: "Shield.qml"; visible: true }
                    }
                ]

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
                            duration: FluidUi.Units.longDuration
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

                FluidUi.Loadable {
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

                FluidUi.Loadable {
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

                ScreenView {
                    id: screenView
                    anchors.fill: parent
                    transform: Scale {
                        id: screenScaler
                        origin.x: zoomArea.x2
                        origin.y: zoomArea.y2
                        xScale: zoomArea.zoom2
                        yScale: zoomArea.zoom2
                    }
                    z: 800

                    ScreenZoom {
                        id: zoomArea
                        anchors.fill: parent
                        scaler: screenScaler
                        enabled: false
                    }

                    Component.onCompleted: mainItem.state = "session"
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

            GreenIsland.PointerItem {
                id: cursor
                inputDevice: output.compositor.defaultInputDevice
                x: localPointerTracker.mouseX - hotspotX
                y: localPointerTracker.mouseY - hotspotY
                visible: globalPointerTracker.output === output &&
                         output.powerState === GreenIsland.WaylandOutput.PowerStateOn
            }
        }
    }

    /*
     * Methods
     */

    function wake() {
        blackRect.fadeOut();
        output.powerState = GreenIsland.WaylandOutput.PowerStateOn;
    }

    function idle() {
        blackRect.fadeIn();
    }
}
