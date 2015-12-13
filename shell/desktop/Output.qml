/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Window 2.2
import GreenIsland 1.0 as GreenIsland
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import "../screens"

GreenIsland.WaylandOutput {
    readonly property bool primary: hawaiiCompositor.primaryScreen === nativeScreen

    readonly property alias surfacesArea: screenView.currentWorkspace
    readonly property alias screenView: screenView

    property QtObject activeWindow: null

    property alias showFps: fpsIndicator.visible
    property alias showInformation: outputInfo.visible
    property alias zoomEnabled: zoomArea.enabled

    id: output
    manufacturer: nativeScreen.manufacturer
    model: nativeScreen.model
    position: nativeScreen.position
    physicalSize: nativeScreen.physicalSize
    subpixel: nativeScreen.subpixel
    transform: nativeScreen.transform
    scaleFactor: nativeScreen.scaleFactor
    sizeFollowsWindow: true
    window: Window {
        id: window
        x: nativeScreen.position.x
        y: nativeScreen.position.y
        width: nativeScreen.size.width
        height: nativeScreen.size.height
        flags: Qt.FramelessWindowHint

        Connections {
            target: SessionInterface
            onSessionLocked: mainItem.state = "lock"
            onSessionUnlocked: mainItem.state = "session"
            onShutdownRequestCanceled: mainItem.state = "session"
            onLogOutRequested: mainItem.state = "logout"
            onPowerOffRequested: mainItem.state = "poweroff"
            onRestartRequested: mainItem.state = "restart"
        }

        GreenIsland.LocalPointerTracker {
            id: localPointerTracker
            anchors.fill: parent
            globalTracker: globalPointerTracker

            Item {
                id: mainItem
                anchors.fill: parent
                state: "splash"
                states: [
                    State {
                        name: "splash"
                        PropertyChanges { target: cursor; visible: false }
                        PropertyChanges { target: splashScreen; opacity: 1.0 }
                        //PropertyChanges { target: keyFilter; enabled: false }
                        //StateChangeScript { script: disableInput() }
                    },
                    State {
                        name: "session"
                        PropertyChanges { target: cursor; visible: true }
                        //PropertyChanges { target: keyFilter; enabled: true }
                        PropertyChanges { target: splashScreen; opacity: 0.0 }
                        PropertyChanges { target: screenView.windowsSwitcherLoader; active: false }
                        //PropertyChanges { target: shieldLoader; source: ""; visible: false }
                        //PropertyChanges { target: logoutLoader; loadComponent: false }
                        //PropertyChanges { target: lockScreenLoader; loadComponent: false }
                        PropertyChanges { target: splashScreen; opacity: 0.0 }
                        //StateChangeScript { script: enableInput() }
                    },
                    State {
                        name: "logout"
                        PropertyChanges { target: cursor; visible: true }
                        //PropertyChanges { target: keyFilter; enabled: false }
                        //PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
                        PropertyChanges { target: logoutLoader; loadComponent: true; mode: "logout" }
                        //StateChangeScript { script: disableInput() }
                    },
                    State {
                        name: "poweroff"
                        PropertyChanges { target: cursor; visible: true }
                        //PropertyChanges { target: keyFilter; enabled: false }
                        //PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
                        PropertyChanges { target: logoutLoader; loadComponent: true; mode: "poweroff" }
                        //StateChangeScript { script: disableInput() }
                    },
                    State {
                        name: "restart"
                        PropertyChanges { target: cursor; visible: true }
                        //PropertyChanges { target: keyFilter; enabled: false }
                        //PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
                        PropertyChanges { target: logoutLoader; loadComponent: true; mode: "restart" }
                        //StateChangeScript { script: disableInput() }
                    },
                    State {
                        name: "lock"
                        PropertyChanges { target: cursor; visible: true }
                        //PropertyChanges { target: keyFilter; enabled: false }
                        PropertyChanges { target: screenView.windowsSwitcherLoader; active: false }
                        //PropertyChanges { target: shieldLoader; source: ""; visible: false }
                        PropertyChanges { target: logoutLoader; loadComponent: false }
                        PropertyChanges { target: lockScreenLoader; loadComponent: true }
                        //StateChangeScript { script: disableInput() }
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
                    opacity: 0.0
                    z: 900
                    onOpacityChanged: {
                        if (opacity == 1.0)
                            splashScreen.source = "../screens/SplashScreen.qml";
                        else if (opacity == 0.0)
                            splashScreenTimer.start();
                    }

                    // Unload after a while so that the opacity animation is visible
                    Timer {
                        id: splashScreenTimer
                        running: false
                        interval: 5000
                        onTriggered: {
                            splashScreen.source = "";
                        }
                    }

                    Behavior on opacity {
                        NumberAnimation {
                            easing.type: Easing.InSine
                            duration: Themes.Units.longDuration
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

                Components.Loadable {
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

                Components.Loadable {
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
                    model: output.model
                    primary: output.primary
                    z: 1000
                    visible: false
                }
            }

            GreenIsland.PointerItem {
                id: cursor
                inputDevice: output.compositor.defaultInputDevice
                x: localPointerTracker.mouseX - hotspotX
                y: localPointerTracker.mouseY - hotspotY
                visible: globalPointerTracker.output === output
            }
        }
    }
}
