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
import Hawaii.Themes 1.0 as Themes

GreenIsland.WaylandOutput {
    readonly property bool primary: hawaiiCompositor.primaryScreen === nativeScreen

    property alias surfacesArea: screenView.currentWorkspace

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

        GreenIsland.KeyBindingsFilter {
            anchors.fill: parent
            keyBindings: hawaiiCompositor.keyBindingsManager
        }

        GreenIsland.WaylandMouseTracker {
            id: mouseTracker
            anchors.fill: parent
            enableWSCursor: true

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
                        //PropertyChanges { target: windowSwitcherLoader; active: false }
                        //PropertyChanges { target: shieldLoader; source: ""; visible: false }
                        //PropertyChanges { target: logoutLoader; loadComponent: false }
                        //PropertyChanges { target: lockScreenLoader; loadComponent: false }
                        PropertyChanges { target: splashScreen; opacity: 0.0 }
                        //StateChangeScript { script: enableInput() }
                    },
                    State {
                        name: "windowSwitcher"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: windowSwitcherLoader; active: true }
                        StateChangeScript { script: disableInput() }
                    },
                    State {
                        name: "logout"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: keyFilter; enabled: false }
                        //PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
                        PropertyChanges { target: logoutLoader; loadComponent: true; mode: "logout" }
                        StateChangeScript { script: disableInput() }
                    },
                    State {
                        name: "poweroff"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: keyFilter; enabled: false }
                        //PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
                        PropertyChanges { target: logoutLoader; loadComponent: true; mode: "poweroff" }
                        StateChangeScript { script: disableInput() }
                    },
                    State {
                        name: "restart"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: keyFilter; enabled: false }
                        //PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
                        PropertyChanges { target: logoutLoader; loadComponent: true; mode: "restart" }
                        StateChangeScript { script: disableInput() }
                    },
                    State {
                        name: "lock"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: keyFilter; enabled: false }
                        PropertyChanges { target: windowSwitcherLoader; active: false }
                        PropertyChanges { target: shieldLoader; source: ""; visible: false }
                        PropertyChanges { target: logoutLoader; loadComponent: false }
                        PropertyChanges { target: lockScreenLoader; loadComponent: true }
                        StateChangeScript { script: disableInput() }
                    },
                    State {
                        name: "shield"
                        PropertyChanges { target: cursor; visible: true }
                        PropertyChanges { target: shieldLoader; source: "Shield.qml"; visible: true }
                    }
                ]

                Loader {
                    id: splashScreen
                    anchors.fill: parent
                    opacity: 0.0
                    onOpacityChanged: {
                        if (opacity == 1.0) {
                            splashScreen.z = 910;
                            splashScreen.source = "../SplashScreen.qml";
                        } else if (opacity == 0.0) {
                            splashScreenTimer.start();
                        }
                    }

                    // Unload after a while so that the opacity animation is visible
                    Timer {
                        id: splashScreenTimer
                        running: false
                        interval: 5000
                        onTriggered: {
                            splashScreen.z = 899;
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

                    ScreenZoom {
                        id: zoomArea
                        anchors.fill: parent
                        scaler: screenScaler
                        enabled: false
                    }

                    Component.onCompleted: mainItem.state = "session"
                }

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
                    visible: false
                }
            }

            GreenIsland.WaylandCursorItem {
                id: cursor
                inputDevice: output.compositor.defaultInputDevice
                inputEventsEnabled: false
                x: mouseTracker.mouseX - hotspotX
                y: mouseTracker.mouseY - hotspotY
            }
        }
    }
}
