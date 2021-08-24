// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls
import "components" as Components

Components.Dialog {
    property date startTime
    property int remainingSeconds: 60
    property int totalSeconds: 60

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    title: qsTr("Log out")

    onOpened: {
        startTime = new Date();
        remainingSeconds = totalSeconds - (new Date() - startTime) / 1000;
    }

    onAccepted: {
        shell.terminateCompositor();
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
            accept();
        }
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
