/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.4
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
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
            Qt.quit();
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
        onAccepted: Qt.quit()
        onRejected: logoutScreen.canceled()
    }
}
