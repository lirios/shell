// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls

Dialog {
    modal: true
    focus: true

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    standardButtons: Dialog.Cancel

    ColumnLayout {
        width: parent.width
        spacing: FluidControls.Units.smallSpacing

        Label {
            text: qsTr("Enter a Command")
        }

        TextField {
            focus: true
            onAccepted: {
                SessionInterface.launchCommand(text);
                text = "";
                close();
            }

            Layout.minimumWidth: 350
            Layout.fillWidth: true
        }
    }
}
