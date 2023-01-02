// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Aurora.Compositor 1.0 as Compositor
import Liri.Shell.Compositor 1.0 as LS
import Fluid.Controls 1.0 as FluidControls

Window {
    id: outputWindow

    readonly property alias lockSurfacesModel: lockSurfacesModel
    readonly property alias layerSurfacesModel: layerSurfacesModel
    readonly property alias currentWorkspace: workspacesView.currentWorkspace

    readonly property alias splashVisible: splash.visible
    property alias showFps: fpsIndicator.visible
    property alias showInformation: outputInfo.visible

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    x: output.position.x
    y: output.position.y
    width: output.geometry.width
    height: output.geometry.height
    flags: Qt.Window | Qt.FramelessWindowHint
    screen: output.screen ? Qt.application.screens[output.screen.screenIndex] : null
    color: splashVisible
           ? Material.color(Material.BlueGrey, Material.Shade800)
           : !splashVisible && shellHelper.isReady ? Material.color(Material.Grey, Material.Shade700) : "black"
    visible: output.screen && output.screen.enabled

    // Keyboard handling
    LS.KeyEventFilter {
        Keys.onPressed: {
            // Input wakes the output
            liriCompositor.wake();

            // Handle Meta modifier
            if (event.modifiers & Qt.MetaModifier) {
                // Open window switcher
                if (output.primary) {
                    if (event.key === Qt.Key_Tab) {
                        event.accepted = true;
                        windowSwitcher.next();
                        return;
                    } else if (event.key === Qt.Key_Backtab) {
                        event.accepted = true;
                        windowSwitcher.previous();
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
                shellHelper.requestShutdown();
                event.accepted = true;
                return;
            default:
                break;
            }

            // Ignore the event
            event.accepted = false;
        }

        Keys.onReleased: {
            // Input wakes the output
            liriCompositor.wake();

            // Handle Meta modifier
            if (event.modifiers & Qt.MetaModifier) {
                // Close window switcher
                if (output.primary) {
                    if (event.key === Qt.Key_Super_L || event.key === Qt.Key_Super_R) {
                        event.accepted = true;
                        windowSwitcher.close();
                        windowSwitcher.activate();
                        return;
                    }
                }
            }

            // Ignore the event
            event.accepted = false;
        }
    }

    // Mouse tracker
    Compositor.WaylandMouseTracker {
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
        LS.WaylandCursorGrabber {
            seat: liriCompositor.defaultSeat
            grab: mouseTracker.containsMouse
        }

        // Layout
        LS.DesktopLayout {
            id: desktopLayout

            anchors.fill: parent

            // Workspaces
            WorkspacesView {
                id: workspacesView

                anchors.fill: parent
                objectName: "workspaces"
            }

            // Lock surfaces
            Repeater {
                model: ListModel {
                    id: lockSurfacesModel
                }

                LockScreenItem {
                    objectName: "lockscreen"
                    shellSurface: model.lockSurface

                    onDestroyAnimationFinished: {
                        lockSurfacesModel.remove(index);
                    }
                }
            }

            // Layer surfaces
            Repeater {
                model: ListModel {
                    id: layerSurfacesModel
                }

                LayerSurfaceItem {
                    layerSurface: model.layerSurface
                    output: model.output

                    onDestroyAnimationFinished: {
                        layerSurfacesModel.remove(index);
                    }
                }
            }

            // Window switcher
            WindowSwitcher {
                id: windowSwitcher

                x: (parent.width - width) / 2
                y: (parent.height - height) / 2
                objectName: "window-switcher"
            }

            // Virtual Keyboard
            Loader {
                active: liriCompositor.settings.ui.inputMethod === "qtvirtualkeyboard"
                source: Qt.resolvedUrl("Keyboard.qml")
                x: (parent.width - width) / 2
                y: parent.height - height
                width: Math.max(parent.width / 2, 768)
            }

            // Black bands around fullscreen surfaces
            Rectangle {
                id: fullScreenLayer

                anchors.fill: parent
                objectName: "fullscreen"
                color: "black"
                visible: children.length
            }

            // Modal overlay
            ModalOverlay {
                objectName: "modal-overlay"
            }
        }

        // Top-left corner
        HotCorner {
            corner: Qt.TopLeftCorner
        }

        // Top-right corner
        HotCorner {
            corner: Qt.TopRightCorner
        }

        // Bottom-left corner
        HotCorner {
            corner: Qt.BottomLeftCorner
            onTriggered: {
                if (currentWorkspace.state === "normal")
                    currentWorkspace.state = "present";
                else
                    currentWorkspace.state = "normal";
            }
        }

        // Bottom-right corner
        HotCorner {
            corner: Qt.BottomRightCorner
        }

        // Hide the other surfaces, just in case the session locker crashes
        SessionLocker {
            anchors.fill: parent
            visible: sessionLockManager.locked && !sessionLockManager.hasClientConnected
        }

        // Idle dimmer
        IdleDimmer {
            id: idleDimmer

            anchors.fill: parent
        }

        // Flash for screenshots
        Flash {
            id: flash
        }

        // Splash screen
        FluidControls.Wave {
            id: splash

            anchors.fill: parent

            Rectangle {
                anchors.fill: parent
                color: Material.color(Material.BlueGrey, Material.Shade800)
            }

            Component.onCompleted: {
                openWave(0, 0);
            }
        }

        // Zoom with mouse wheel
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

        // FPS
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
            visible: false

            LS.FpsCounter {
                id: fpsCounter
            }
        }

        // Output information for debugging purpose
        OutputInfo {
            id: outputInfo

            anchors {
                left: parent.left
                top: parent.top
            }
            visible: false
        }
    }

    /*
     * Methods
     */

    function reveal() {
        splash.closeWave(splash.width - splash.size, splash.height - splash.size);
    }

    function flash() {
        flash.flash();
    }

    function selectPreviousWorkspace() {
        workspacesView.selectPrevious();
    }

    function selectNextWorkspace() {
        workspacesView.selectNext();
    }

    function selectWorkspace(num) {
        workspacesView.select(num);
    }
}
