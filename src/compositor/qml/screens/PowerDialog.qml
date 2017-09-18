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

import QtQuick 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0

Popup {
    id: powerDialog

    property string actionTitle
    property string actionText
    property string actionIcon

    property var startTime
    property int remainingSeconds
    property int totalSeconds: 60
    property bool accepted: false

    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    padding: 0

    x: (output.availableGeometry.width - width)/2
    y: (output.availableGeometry.height - height)/2

    parent: screenView

    modal: true

    Material.theme: Material.Light
    Material.accent: Material.Blue

    signal actionTriggered()
    signal canceled()

    onOpened: {
        startTime = new Date()
        remainingSeconds = totalSeconds - (new Date() - startTime)/1000
        accepted = false
    }

    onClosed: {
        if (!accepted)
            canceled()
    }

    function accept() {
        accepted = true
        close()
    }

    Timer {
        running: powerDialog.visible
        interval: 1000
        repeat: true
        onTriggered: {
            remainingSeconds = totalSeconds - (new Date() - startTime)/1000
        }
    }

    Timer {
        running: powerDialog.visible
        interval: totalSeconds * 1000
        onTriggered: {
            powerDialog.accept()
            actionTriggered()
        }
    }

    Column {
        width: 300

        Rectangle {
            radius: 2
            color: Material.color(Material.Red)

            width: parent.width
            height: width * 3/5

            Material.theme: Material.Dark

            Rectangle {
                height: parent.height/2
                color: parent.color

                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
            }

            Column {
                anchors.centerIn: parent
                spacing: 4

                Icon {
                    anchors.horizontalCenter: parent.horizontalCenter
                    name: actionIcon
                    size: 36
                }

                Item {
                    width: parent.width
                    height: 8
                }

                TitleLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: Material.primaryTextColor
                    text: actionTitle
                }

                SubheadingLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: Material.secondaryTextColor
                    horizontalAlignment: Qt.AlignHCenter
                    text: ("The system will %1\nautomatically in %2 " +
                              "seconds.").arg(actionText).arg(remainingSeconds)
                }
            }
        }

        Item {
            width: parent.width
            height: 8
        }

        ListItem {
            iconSource: Qt.resolvedUrl("../images/sleep.svg")
            text: qsTr("Sleep")
            visible: SessionInterface.canSuspend
            onClicked: {
                powerDialog.accept()
                // TODO: Lock screen
                SessionInterface.suspend()
            }
        }

        ListItem {
            iconName: "file/file_download"
            text: qsTr("Suspend to disk")
            visible: SessionInterface.canHibernate
            onClicked: {
                powerDialog.accept()
                SessionInterface.hibernate()
            }
        }

        ListItem {
            iconName: "action/power_settings_new"
            text: qsTr("Power off")
            visible: SessionInterface.canPowerOff
            onClicked: {
                powerDialog.accept()
                SessionInterface.powerOff()
            }
        }

        ListItem {
            iconSource: Qt.resolvedUrl("../images/reload.svg")
            text: qsTr("Restart")
            visible: SessionInterface.canRestart
            onClicked: {
                powerDialog.accept()
                SessionInterface.restart()
            }
        }

        Item {
            width: parent.width
            height: 8
        }
    }
}
