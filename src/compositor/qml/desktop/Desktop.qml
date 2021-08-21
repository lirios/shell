// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtWayland.Compositor 1.0 as QtWaylandCompositor
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects
import Liri.WaylandServer 1.0 as WS
import Liri.Session 1.0 as Session
import Liri.Shell 1.0 as LS
import Liri.private.shell 1.0 as P
import ".."
import "../indicators" as Indicators
import "../screens" as Screens
import "../windows" as Windows

Item {
    id: desktop

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    property bool cursorVisible: true

    readonly property alias backgroundLayer: backgroundLayer
    readonly property alias bottomLayer: bottomLayer
    readonly property alias topLayer: topLayer
    readonly property alias overlayLayer: overlayLayer

    readonly property alias backgroundLayerModel: backgroundLayerModel
    readonly property alias bottomLayerModel: bottomLayerModel
    readonly property alias topLayerModel: topLayerModel
    readonly property alias overlayLayerModel: overlayLayerModel
    readonly property alias modalOverlayModel: modalOverlayModel

    readonly property alias workspacesView: workspacesView
    readonly property alias currentWorkspace: workspacesView.currentWorkspace

    readonly property var layers: QtObject {
        readonly property alias fullScreen: fullScreenLayer
    }

    readonly property alias shell: shell
    readonly property var panel: shell.panel
    readonly property alias windowSwitcher: windowSwitcher

    property alias showFps: fpsIndicator.visible
    property alias showInformation: outputInfo.visible

    state: "splash"
    states: [
        State {
            name: "splash"
            PropertyChanges { target: desktop; cursorVisible: false }
            PropertyChanges { target: splashScreen; opacity: 1.0 }
        },
        State {
            name: "session"
            PropertyChanges { target: desktop; cursorVisible: true }
        }
    ]

    Connections {
        target: Session.SessionManager

        function onSessionLocked() {
            // Ask the client to show the lock screen
            lockScreen.requestLock();
        }
        function onSessionUnlocked() {
            desktop.state = "session";
            output.locked = false;
        }
        function onIdleInhibitRequested() {
            liriCompositor.idleInhibit++;
        }
        function onIdleUninhibitRequested() {
            liriCompositor.idleInhibit--;
        }
    }

    /*
     * Workspace
     */

    // Background
    Item {
        id: backgroundLayer

        anchors.fill: parent

        Repeater {
            model: ListModel {
                id: backgroundLayerModel
            }

            HardwareLayerSurfaceItem {
                stackingLevel: -1
                layerSurface: model.layerSurface
                output: model.output

                onDestroyAnimationFinished: {
                    backgroundLayerModel.remove(index);
                }
            }
        }
    }

    // Bottom
    Item {
        id: bottomLayer

        anchors.fill: parent

        Repeater {
            model: ListModel {
                id: bottomLayerModel
            }

            LayerSurfaceItem {
                layerSurface: model.layerSurface
                output: model.output

                onDestroyAnimationFinished: {
                    bottomLayerModel.remove(index);
                }
            }
        }
    }

    // Workspaces
    WorkspacesView {
        id: workspacesView
    }

    // Top
    Item {
        id: topLayer

        anchors.fill: parent

        Repeater {
            model: ListModel {
                id: topLayerModel
            }

            LayerSurfaceItem {
                layerSurface: model.layerSurface
                output: model.output

                onDestroyAnimationFinished: {
                    topLayerModel.remove(index);
                }
            }
        }
    }

    // Overlays
    Item {
        id: overlayLayer

        anchors.fill: parent

        Repeater {
            model: ListModel {
                id: overlayLayerModel
            }

            LayerSurfaceItem {
                layerSurface: model.layerSurface
                output: model.output

                onCreateAnimationFinished: {
                    if (layerSurface.nameSpace === "lockscreen") {
                        // Make sure all surfaces are hidden
                        output.locked = true;

                        // FIXME: Before suspend we lock the screen, but turning the output off has a side effect:
                        // when the system is resumed it won't flip so we comment this out but unfortunately
                        // it means that the lock screen will not turn off the screen
                        //output.idle();
                    }
                }

                onDestroyAnimationFinished: {
                    overlayLayerModel.remove(index);
                }
            }
        }
    }

    // Panels
    Shell {
        id: shell

        anchors.fill: parent
        opacity: currentWorkspace.state == "present" ? 0.0 : 1.0
        visible: output.primary

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: 250
            }
        }
    }

    // Full screen windows can cover application windows and panels
    Rectangle {
        id: fullScreenLayer

        anchors.fill: parent
        color: "black"
        opacity: children.length > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: FluidControls.Units.mediumDuration
            }
        }
    }

    Rectangle {
        id: modalOverlay

        Material.theme: Material.Light

        anchors.fill: parent
        color: Material.backgroundDimColor
        opacity: liriCompositor.showModalOverlay ? 1.0 : 0.0
        enabled: liriCompositor.showModalOverlay

        Behavior on opacity {
            NumberAnimation { duration: 150 }
        }

        HoverHandler {
            grabPermissions: HoverHandler.CanTakeOverFromAnything
        }

        TapHandler {
            grabPermissions: TapHandler.CanTakeOverFromAnything
            onTapped: {
                liriModal.sendDone();
            }
        }

        Repeater {
            model: ListModel {
                id: modalOverlayModel
            }

            LayerSurfaceItem {
                layerSurface: model.layerSurface
                output: model.output

                onDestroyAnimationFinished: {
                    overlayLayerModel.remove(index);
                }
            }
        }
    }

    /*
     * Windows switcher
     */

    WindowSwitcher {
        id: windowSwitcher

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
    }

    /*
     * Hot corners
     */

    Item {
        anchors.fill: parent

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
                if (workspacesView.currentWorkspace.state === "normal")
                    workspacesView.currentWorkspace.state = "present";
                else
                    workspacesView.currentWorkspace.state = "normal";
            }
        }

        // Bottom-right corner
        HotCorner {
            corner: Qt.BottomRightCorner
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
            shellHelper.requestShutdown();
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
}
