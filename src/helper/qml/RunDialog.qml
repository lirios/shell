// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls
import Liri.Session 1.0 as Session
import "components" as Components

Components.Dialog {
    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    standardButtons: Dialog.Cancel

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
                accept();
            }

            Layout.minimumWidth: 350
            Layout.fillWidth: true
        }
    }
}
