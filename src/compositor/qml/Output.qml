// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtWayland.Compositor 1.15
import Liri.Shell 1.0 as LiriShell
import Liri.private.shell 1.0 as P
import "desktop"

P.WaylandOutput {
    id: output

    readonly property bool primary: liriCompositor.defaultOutput === this
    property bool locked: false

    property var screen: null

    property var viewsBySurface: ({})

    property int idleInhibit: 0

    //readonly property alias idleDimmer: idleDimmer
    readonly property alias desktop: desktop

    readonly property alias hardwareLayerSurfaceModel: desktop.hardwareLayerSurfaceModel
    readonly property alias layerSurfaceModel: desktop.layerSurfaceModel
    readonly property alias modalOverlayModel: desktop.modalOverlayModel

    readonly property alias currentWorkspace: desktop.currentWorkspace

    property alias showFps: desktop.showFps
    property alias showInformation: desktop.showInformation

    property var exportDmabufFrame: null

    property bool __idle: false

    sizeFollowsWindow: false
    automaticFrameCallback: screen && screen.enabled && screen.powerState === P.ScreenItem.PowerStateOn

    Connections {
        target: output.screen

        function onCurrentModeChanged(resolution, refreshRate) {
            output.setCurrentOutputMode(resolution, refreshRate);
        }
    }

    Component.onCompleted: {
        if (output.screen) {
            for (var i = 0; i < output.screen.modes.length; i++) {
                var screenMode = output.screen.modes[i];
                var isPreferred = output.screen.preferredMode.resolution === screenMode.resolution &&
                        output.screen.preferredMode.refreshRateRate === screenMode.refreshRate;
                var isCurrent = output.screen.currentMode.resolution === screenMode.resolution &&
                        output.screen.currentMode.refreshRate === screenMode.refreshRate;
                output.addOutputMode(screenMode.resolution, screenMode.refreshRate, isPreferred, isCurrent);
            }
        }
    }

    window: Window {
        id: outputWindow

        x: output.position.x
        y: output.position.y
        width: output.geometry.width
        height: output.geometry.height
        flags: Qt.Window | Qt.FramelessWindowHint
        screen: output.screen ? Qt.application.screens[output.screen.screenIndex] : null
        color: desktop.splashVisible ? Material.color(Material.BlueGrey, Material.Shade800) : "black"
        visible: output.screen.enabled

        // Keyboard handling
        P.KeyEventFilter {
            Keys.onPressed: {
                // Input wakes the output
                liriCompositor.wake();

                desktop.handleKeyPressed(event);
            }

            Keys.onReleased: {
                // Input wakes the output
                liriCompositor.wake();

                desktop.handleKeyReleased(event);
            }
        }

        // Mouse tracker
        WaylandMouseTracker {
            id: mouseTracker

            anchors.fill: parent

            windowSystemCursorEnabled: mouseTracker.containsMouse

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

            // Grab pointer cursor surface and set a bitmap cursor on this window
            P.WaylandCursorGrabber {
                seat: output.compositor.defaultSeat
                grab: mouseTracker.containsMouse
            }

            // User interface
            Desktop {
                id: desktop

                objectName: "desktop"
                anchors.fill: parent
            }

            // Virtual Keyboard
            Loader {
                active: liriCompositor.settings.ui.inputMethod === "qtvirtualkeyboard"
                source: Qt.resolvedUrl("base/Keyboard.qml")
                x: (parent.width - width) / 2
                y: parent.height - height
                width: Math.max(parent.width / 2, 768)
            }

            // Flash for screenshots
            Rectangle {
                id: flash

                anchors.fill: parent

                color: "white"
                opacity: 0.0

                SequentialAnimation {
                    id: flashAnimation

                    OpacityAnimator {
                        easing.type: Easing.OutQuad
                        target: flash
                        from: 0.0
                        to: 1.0
                        duration: 250
                    }
                    OpacityAnimator {
                        easing.type: Easing.OutQuad
                        target: flash
                        from: 1.0
                        to: 0.0
                        duration: 250
                    }
                }
            }

            // Idle dimmer
            IdleDimmer {
                id: idleDimmer

                anchors.fill: parent

                output: output
            }

            Item {
                anchors.fill: parent

                WheelHandler {
                    id: zoomWheelHandler

                    readonly property real minZoom: 1.0
                    readonly property real maxZoom: 10.0
                    readonly property real zoomIncrement: 0.1

                    target: outputWindow.contentItem
                    acceptedModifiers: Qt.ControlModifier | liriCompositor.settings.windowActionModifier
                    acceptedButtons: Qt.NoButton
                    orientation: Qt.Vertical
                    grabPermissions: PointerHandler.CanTakeOverFromAnything
                    onWheel: {
                        if (event.angleDelta.y > 0)
                            target.scale = Math.min(target.scale + zoomIncrement, maxZoom);
                        else if (event.angleDelta.y <= 0)
                            target.scale = Math.max(target.scale - zoomIncrement, minZoom);
                    }
                }
            }
        }
    }

    /*
     * Methods
     */

    function reveal() {
        desktop.reveal();
    }

    function wake() {
        if (!__idle)
            return;

        console.debug("Power on output", manufacturer, model);
        idleDimmer.fadeOut();
        screen.powerState = P.ScreenItem.PowerStateOn;
        __idle = false;
    }

    function idle() {
        if (__idle)
            return;

        console.debug("Standby output", manufacturer, model);
        idleDimmer.fadeIn();
        __idle = true;
    }

    function flash() {
        flashAnimation.start();
    }

    function selectPreviousWorkspace() {
        desktop.workspacesView.selectPrevious();
    }

    function selectNextWorkspace() {
        desktop.workspacesView.selectNext();
    }

    function selectWorkspace(num) {
        desktop.workspacesView.select(num);
    }
}
