// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import Fluid.Controls as FluidControls
import Fluid.Effects as FluidEffects
import Liri.Shell as Shell
import Liri.Shell.LayerShell as LayerShell

Window {
    id: osdWindow

    property string iconName
    property alias value: progress.value
    property alias text: label.text
    property alias progressVisible: progress.visible

    width: 256 + (control.Material.elevation * 4)
    height: 256 + (control.Material.elevation * 4)

    screen: primaryScreen
    color: "transparent"
    visible: true

    LayerShell.Window.layer: LayerShell.Window.LayerOverlay
    LayerShell.Window.keyboardInteractivity: LayerShell.Window.KeyboardInteractivityNone
    LayerShell.Window.scope: "osd"

    FluidControls.InputRegion {
        id: inputRegion
    }

    FluidControls.InputArea {
        // Apparently a null region is not set by QtWayland, so we have to
        // set a 1x1 rectangle that is so small that is never going to be noticed
        region: inputRegion
        width: 1
        height: 1
    }

    Timer {
        id: timer

        interval: 2500
        onTriggered: {
            hideAnimation.start();
        }
    }

    SequentialAnimation {
        id: showAnimation

        alwaysRunToEnd: true

        OpacityAnimator {
            target: control
            duration: 150
            from: 0.0
            to: 1.0
        }

        ScriptAction {
            script: {
                timer.restart();
            }
        }
    }

    SequentialAnimation {
        id: hideAnimation

        alwaysRunToEnd: true

        OpacityAnimator {
            target: control
            duration: 150
            from: 1.0
            to: 1.0
        }

        ScriptAction {
            script: {
                osdWindow.visible = false;
            }
        }
    }

    Control {
        id: control

        Material.theme: Material.Dark
        Material.primary: Material.Blue
        Material.accent: Material.Blue
        Material.elevation: 8

        anchors.fill: parent
        anchors.margins: Material.elevation * 2

        background: Rectangle {
            radius: 2
            color: control.Material.dialogColor

            layer.enabled: true
            layer.effect: FluidEffects.Elevation {
                elevation: control.Material.elevation
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: FluidControls.Units.largeSpacing
            spacing: FluidControls.Units.smallSpacing

            FluidControls.Icon {
                id: icon

                source: osdWindow.iconName ? FluidControls.Utils.iconUrl(osdWindow.iconName) : ""
                size: FluidControls.Units.iconSizes.enormous

                Layout.alignment: Qt.AlignHCenter
            }

            ProgressBar {
                id: progress

                from: 0
                to: 100
                visible: false

                Layout.fillWidth: true
            }

            Label {
                id: label

                visible: !progress.visible && text != ""

                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    function showWindow() {
        if (visible) {
            timer.restart();
        } else {
            control.opacity = 0;
            visible = true;
            showAnimation.start();
        }
    }
}
