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

Dialog {
    width: Screen.width * 0.4
    height: Screen.height * 0.3

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

                Label {
                    text: qsTr("The system will be powered off within 60 seconds.")
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
            }

            Button {
                text: qsTr("Restart")
            }

            Button {
                text: qsTr("Power Off")
            }

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
        }
    }
}
