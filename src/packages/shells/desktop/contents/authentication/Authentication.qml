/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Shell.Components 1.0

Dialog {
    id: authenticationDialog

    property string actionId
    property alias message: messageLabel.text
    property string iconName
    property alias realName: avatarName.text
    property string avatar
    property alias prompt: promptLabel.text
    property bool echo: false
    property alias infoMessage: infoLabel.text
    property alias errorMessage: errorLabel.text
    property var palette: SystemPalette {}

    signal authenticationReady(string response)
    signal authenticationCanceled()

    onRejected: authenticationCanceled()

    ColumnLayout {
        RowLayout {
            FluidUi.Icon {
                iconName: "dialog-password-symbolic"
                width: 24
                height: 24
                color: palette.text

                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                Label {
                    text: qsTr("Authentication required")
                    font.bold: true
                }

                Label {
                    id: messageLabel
                    wrapMode: Text.WordWrap

                    Layout.fillWidth: true
                }

                RowLayout {
                    Image {
                        id: avatarImage
                        source: {
                            // Fallback to a default icon
                            if (avatar == "")
                                return "image://desktoptheme/avatar-default";

                            // Load the image from the disk if it's an absolute path
                            if (avatar.indexOf("/") == 0)
                                return avatar;

                            // Load from the icon theme
                            return "image://desktoptheme/" + avatar;
                        }
                        sourceSize.width: width
                        sourceSize.height: height
                        width: 64
                        height: 64
                        smooth: true
                    }

                    Label {
                        id: avatarName

                        Layout.alignment: Qt.AlignVCenter
                    }

                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth: true
                }

                RowLayout {
                    Label {
                        id: promptLabel

                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    TextField {
                        id: passwordInput
                        echoMode: echo ? TextInput.Normal : TextInput.Password

                        Layout.fillWidth: true
                    }
                }

                Label {
                    id: infoLabel
                    color: "green"
                    font.bold: true
                    wrapMode: Text.WordWrap
                    visible: text != ""

                    Layout.fillWidth: true
                }

                Label {
                    id: errorLabel
                    color: "red"
                    font.bold: true
                    wrapMode: Text.WordWrap
                    visible: text != ""

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
                onClicked: authenticationDialog.authenticationCanceled()
            }

            Button {
                text: qsTr("Authenticate")
                onClicked: authenticationDialog.authenticationReady(passwordInput.text)
            }

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
        }
    }
}
