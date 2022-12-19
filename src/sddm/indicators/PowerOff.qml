/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Liri.Shell 1.0 as LiriShell
import Fluid.Controls 1.0 as FluidControls

LiriShell.Indicator {
    property var startTime
    property int remainingSeconds
    property int totalSeconds: 60

    iconSource: FluidControls.Utils.iconUrl("action/power_settings_new")

    //: Shut down indicator tooltip
    //~ Indicator to shut the system down from SDDM
    tooltip: qsTr("Shut down")

    //: Shut down indicator accessibility name
    //~ Indicator to shut the system down from SDDM
    Accessible.name: qsTr("Shut down the system")

    active: popup.visible
    onClicked: popup.open()

    Popup {
        id: popup

        parent: root
        modal: true

        x: (greeter.width - width) / 2
        y: (greeter.height - height) / 2
        height: 400

        padding: 0

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        Material.primary: Material.color(Material.Blue, Material.Shade500)
        Material.accent: Material.color(Material.Blue, Material.Shade500)

        onOpened: {
            startTime = new Date();
            remainingSeconds = totalSeconds - (new Date() - startTime)/1000;
        }

        Timer {
            running: popup.visible
            interval: 1000
            repeat: true
            onTriggered: {
                remainingSeconds = totalSeconds - (new Date() - startTime)/1000;
            }
        }

        Timer {
            running: popup.visible
            interval: totalSeconds * 1000
            onTriggered: {
                sddm.powerOff();
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
                        text: qsTr("The system will power off\nautomatically in %1 " +
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
                visible: sddm.canSuspend
                onClicked: sddm.suspend()
            }

            FluidControls.ListItem {
                icon.source: FluidControls.Utils.iconUrl("file/file_download")
                text: qsTr("Suspend to disk")
                visible: sddm.canHibernate
                onClicked: sddm.hibernate()
            }

            FluidControls.ListItem {
                icon.source: FluidControls.Utils.iconUrl("action/power_settings_new")
                text: qsTr("Power off")
                visible: sddm.canPowerOff
                onClicked: sddm.powerOff()
            }

            FluidControls.ListItem {
                icon.source: Qt.resolvedUrl("../images/reload.svg")
                text: qsTr("Restart")
                visible: sddm.canReboot
                onClicked: sddm.reboot()
            }

            Item {
                width: parent.width
                height: 8
            }
        }
    }
}
