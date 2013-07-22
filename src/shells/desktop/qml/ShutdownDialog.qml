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
import QtQuick.Window 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import FluidUi 0.2 as FluidUi
import Hawaii.Shell.Desktop 0.1

Dialog {
    id: shutdownDialog
    width: Screen.width * 0.4
    height: Screen.height * 0.3
    onVisibleChanged: {
        if (visible) {
            shutdownTimer.restart();
            shutdownTimer.repeat = true;
        }
    }

    property int timeRemaining: 60

    property var powerManager: PowerManager {}

    ColumnLayout {
        anchors.fill: parent

        SystemPalette {
            id: palette
        }

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
                    text: qsTr("Shut Down")
                    font.bold: true
                }

                Timer {
                    id: shutdownTimer
                    interval: 998
                    triggeredOnStart: true
                    repeat: true
                    onTriggered: {
                        timerLabel.text = qsTr("The system will power off automatically " +
                                               "in %1 seconds.").arg(timeRemaining);
                        timeRemaining--;

                        if (timeRemaining == 0) {
                            shutdownTimer.repeat = false;
                            // TODO: Power off!
                        }
                    }
                }

                Label {
                    id: timerLabel
                    wrapMode: Text.WordWrap

                    Layout.fillWidth: true
                }
            }

            Layout.fillHeight: true
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        RowLayout {
            Button {
                text: qsTr("Cancel")
                onClicked: shutdownDialog.visible = false
            }

            Button {
                text: qsTr("Restart")
                enabled: powerManager.capabilities & PowerManager.Restart
            }

            Button {
                text: qsTr("Power Off")
                enabled: powerManager.capabilities & PowerManager.PowerOff
            }

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
        }
    }
}
