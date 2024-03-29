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
import Liri.Session as Session

Window {
    id: runDialogWindow

    color: "transparent"
    visible: false

    LayerSurface.layer: LayerSurface.TopLayer
    LayerSurface.anchors: LayerSurface.LeftAnchor | LayerSurface.RightAnchor |
                LayerSurface.TopAnchor | LayerSurface.BottomAnchor
    LayerSurface.keyboardInteractivity: LayerSurface.ExclusiveKeyboardInteractivity
    LayerSurface.exclusiveZone: -1
    LayerSurface.scope: "run-dialog"

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
