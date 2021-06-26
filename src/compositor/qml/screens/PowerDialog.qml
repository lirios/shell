// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls
import Liri.Device 1.0 as LiriDevice

Popup {
    id: powerDialog

    property var startTime
    property int remainingSeconds
    property int totalSeconds: 60
    property bool accepted: false

    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    padding: 0

    x: (output.availableGeometry.width - width)/2
    y: (output.availableGeometry.height - height)/2

    parent: desktop

    modal: true

    Material.theme: Material.Light
    Material.accent: Material.Blue

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
            LiriDevice.LocalDevice.powerOff()
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

                FluidControls.Icon {
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: FluidControls.Utils.iconUrl("action/power_settings_new")
                    size: 36
                }

                Item {
                    width: parent.width
                    height: 8
                }

                FluidControls.TitleLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: Material.primaryTextColor
                    text: qsTr("Power Off")
                }

                FluidControls.SubheadingLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: Material.secondaryTextColor
                    horizontalAlignment: Qt.AlignHCenter
                    text: ("The system will power off\nautomatically in %1 " +
                              "seconds.").arg(remainingSeconds)
                }
            }
        }

        Item {
            width: parent.width
            height: 8
        }

        FluidControls.ListItem {
            icon.source: Qt.resolvedUrl("../images/sleep.svg")
            text: qsTr("Sleep")
            visible: LiriDevice.LocalDevice.canSuspend
            onClicked: {
                powerDialog.accept()
                LiriDevice.LocalDevice.suspend()
            }
        }

        FluidControls.ListItem {
            icon.source: FluidControls.Utils.iconUrl("file/file_download")
            text: qsTr("Suspend to disk")
            visible: LiriDevice.LocalDevice.canHibernate
            onClicked: {
                powerDialog.accept()
                LiriDevice.LocalDevice.hibernate()
            }
        }

        FluidControls.ListItem {
            icon.source: FluidControls.Utils.iconUrl("action/power_settings_new")
            text: qsTr("Power off")
            visible: LiriDevice.LocalDevice.canPowerOff
            onClicked: {
                powerDialog.accept()
                LiriDevice.LocalDevice.powerOff()
            }
        }

        FluidControls.ListItem {
            icon.source: Qt.resolvedUrl("../images/reload.svg")
            text: qsTr("Restart")
            visible: LiriDevice.LocalDevice.canRestart
            onClicked: {
                powerDialog.accept()
                LiriDevice.LocalDevice.restart()
            }
        }

        Item {
            width: parent.width
            height: 8
        }
    }
}
