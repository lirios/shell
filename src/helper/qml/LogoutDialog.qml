// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Aurora.Client 1.0 as AuroraClient
import Fluid.Controls 1.0 as FluidControls

Window {
    id: logoutDialogWindow

    property date startTime
    property int remainingSeconds: 60
    property int totalSeconds: 60

    color: "transparent"
    visible: false

    AuroraClient.WlrLayerSurfaceV1 {
        layer: AuroraClient.WlrLayerSurfaceV1.TopLayer
        anchors: AuroraClient.WlrLayerSurfaceV1.LeftAnchor |
                 AuroraClient.WlrLayerSurfaceV1.TopAnchor |
                 AuroraClient.WlrLayerSurfaceV1.RightAnchor |
                 AuroraClient.WlrLayerSurfaceV1.BottomAnchor
        keyboardInteractivity: AuroraClient.WlrLayerSurfaceV1.ExclusiveKeyboardInteractivity
        exclusiveZone: -1
        role: "logout-dialog"
    }

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
