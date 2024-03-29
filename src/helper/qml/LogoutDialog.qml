// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import Fluid.Controls as FluidControls
import Liri.Shell as Shell
import Liri.QtShellIntegration

Window {
    id: logoutDialogWindow

    property date startTime
    property int remainingSeconds: 60
    property int totalSeconds: 60

    color: "transparent"
    visible: false

    LayerSurface.layer: LayerSurface.TopLayer
    LayerSurface.anchors: LayerSurface.LeftAnchor | LayerSurface.RightAnchor |
                LayerSurface.TopAnchor | LayerSurface.BottomAnchor
    LayerSurface.keyboardInteractivity: LayerSurface.ExclusiveKeyboardInteractivity
    LayerSurface.exclusiveZone: -1
    LayerSurface.scope: "logout-dialog"

    Dialog {
        id: logoutDialog

        Material.theme: Material.Dark
        Material.primary: Material.Blue
        Material.accent: Material.Blue

        anchors.centerIn: parent

        title: qsTr("Log out")

        focus: true
        modal: true
        visible: logoutDialogWindow.visible

        onOpened: {
            startTime = new Date();
            remainingSeconds = totalSeconds - (new Date() - startTime) / 1000;
        }

        onAccepted: {
            shell.terminateCompositor();
            logoutDialogWindow.hide();
        }
        onRejected: {
            logoutDialogWindow.hide();
        }

        ColumnLayout {
            spacing: FluidControls.Units.smallSpacing

            FluidControls.DialogLabel {
                text: qsTr("You will be logged out in %1 seconds").arg(remainingSeconds)
            }
        }

        footer: DialogButtonBox {
            Button {
                text: qsTr("Log out")
                flat: true

                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            }

            Button {
                text: qsTr("Cancel")
                flat: true

                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            }
        }
    }


    Timer {
        running: visible
        interval: 1000
        repeat: true
        onTriggered: {
            remainingSeconds = totalSeconds - (new Date() - startTime) / 1000;
        }
    }

    Timer {
        running: visible
        interval: totalSeconds * 1000
        onTriggered: {
            logoutDialog.accept();
        }
    }
}
