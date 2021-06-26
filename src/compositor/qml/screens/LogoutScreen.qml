// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls

Item {
    id: logoutScreen

    property bool active
    property var startTime
    property int remainingSeconds
    property int totalSeconds: 60

    anchors.fill: parent

    signal canceled()

    onActiveChanged: {
        if (!output.primary)
            return

        if (active)
            logoutDialog.open()
        else
            logoutDialog.close()
    }

    Timer {
        running: logoutDialog.visible
        interval: 1000
        repeat: true
        onTriggered: {
            remainingSeconds = totalSeconds - (new Date() - startTime)/1000
        }
    }

    Timer {
        running: logoutDialog.visible
        interval: totalSeconds * 1000
        onTriggered: {
            logoutDialog.close()
            liriCompositor.quit();
        }
    }

    Rectangle {
        anchors.fill: parent

        opacity: logoutScreen.active && !output.primary ? 1 : 0
        color: Material.backgroundDimColor

        Behavior on opacity { NumberAnimation { duration: 150 } }
    }

    FluidControls.AlertDialog {
        id: logoutDialog

        Material.primary: Material.Blue
        Material.accent: Material.Blue

        x: (parent.width - width)/2
        y: (parent.height - height)/2

        implicitWidth: 400

        title: qsTr("Log out")
        text: qsTr("You will be logged out in %1 seconds").arg(logoutScreen.remainingSeconds)

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

        onOpened: {
            logoutScreen.startTime = new Date()
            logoutScreen.remainingSeconds = logoutScreen.totalSeconds - (new Date() - logoutScreen.startTime)/1000
        }
        onAccepted: liriCompositor.quit()
        onRejected: logoutScreen.canceled()
    }
}
