/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import FluidUi 0.2 as FluidUi
import Hawaii.Shell 1.0

Dialog {
    id: shutdownDialog

    property variant powerManager: Shell.service("PowerManager")
    property var palette: SystemPalette {}

    onVisibleChanged: shutdownTimer.timeRemaining = 60
    onAccepted: visible = false
    onRejected: visible = false

    Timer {
        property int timeRemaining: 60

        id: shutdownTimer
        interval: 10000
        running: shutdownDialog.visible && timeRemaining > 0
        repeat: shutdownDialog.visible && timeRemaining > 0
        onTriggered: {
            timeRemaining -= 10;
            updateLabel();

            if (timeRemaining <= 0)
                shutdownDialog.powerManager.powerOff();
        }

        function updateLabel() {
            timerLabel.text = qsTr("The system will power off automatically " +
                                   "in %1 seconds.").arg(timeRemaining);
        }
    }

    ColumnLayout {
        RowLayout {
            FluidUi.Icon {
                iconName: "system-shutdown-symbolic"
                width: 24
                height: 24
                color: palette.text

                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                Label {
                    text: qsTr("Power Off")
                    font.bold: true
                }

                Label {
                    id: timerLabel
                    wrapMode: Text.WordWrap

                    Layout.fillWidth: true
                }
            }

            Layout.fillHeight: true
        }

        RowLayout {
            Button {
                text: qsTr("Cancel")
                onClicked: shutdownDialog.visible = false
            }

            Button {
                text: qsTr("Restart")
                enabled: powerManager.capabilities & PowerManager.Restart
                onClicked: powerManager.restart()
            }

            Button {
                text: qsTr("Power Off")
                enabled: powerManager.capabilities & PowerManager.PowerOff
                onClicked: powerManager.powerOff()
            }

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
        }
    }

    Component.onCompleted: shutdownTimer.updateLabel()
}
