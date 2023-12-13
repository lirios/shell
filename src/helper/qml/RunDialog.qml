// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls
import Liri.Shell 1.0 as Shell
import Liri.Session 1.0 as Session

Window {
    id: runDialogWindow

    color: "transparent"
    visible: false

    Shell.WlrLayerSurfaceV1 {
        layer: Shell.WlrLayerSurfaceV1.TopLayer
        anchors: Shell.WlrLayerSurfaceV1.LeftAnchor |
                 Shell.WlrLayerSurfaceV1.TopAnchor |
                 Shell.WlrLayerSurfaceV1.RightAnchor |
                 Shell.WlrLayerSurfaceV1.BottomAnchor
        keyboardInteractivity: Shell.WlrLayerSurfaceV1.ExclusiveKeyboardInteractivity
        exclusiveZone: -1
        role: "run-dialog"
    }

    Dialog {
        id: runDialog

        Material.theme: Material.Dark
        Material.primary: Material.Blue
        Material.accent: Material.Blue

        anchors.centerIn: parent

        standardButtons: Dialog.Cancel

        focus: true
        visible: runDialogWindow.visible

        onAccepted: {
            runDialogWindow.hide();
        }
        onRejected: {
            runDialogWindow.hide();
        }

        ColumnLayout {
            spacing: FluidControls.Units.smallSpacing

            Label {
                text: qsTr("Enter a Command")
            }

            TextField {
                focus: true
                onAccepted: {
                    Session.Launcher.launchCommand(text);
                    text = "";
                    runDialog.accept();
                }

                Layout.minimumWidth: 350
                Layout.fillWidth: true
            }
        }
    }
}
