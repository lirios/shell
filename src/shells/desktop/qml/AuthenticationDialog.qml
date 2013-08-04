/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import FluidUi.ListItems 0.2 as ListItem
import Hawaii.Shell.Desktop 0.1

Dialog {
    id: authenticationDialog

    property string actionId
    property alias message: messageLabel.text
    property string iconName
    property variant details
    property variant defaultIdentity: null
    property variant identities
    property alias prompt: promptLabel.text
    property bool echo: false
    property alias response: passwordInput.text

    property var palette: SystemPalette {}

    ColumnLayout {
        width: Math.max(childrenRect.width, 320)
        height: Math.max(childrenRect.height, 135)

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

                ListView {
                    id: usersView
                    model: identities
                    delegate: ListItem.Standard {
                        // TODO: If it fails to load the icon fallback to avatar-default
                        iconName: modelData.iconFileName
                        text: modelData.displayName
                    }
                    clip: true
                }

                Label {
                    id: promptLabel
                    anchors {
                        left: parent.left
                        verticalCenter: passwordInput.verticalCenter
                    }
                }

                TextField {
                    id: passwordInput
                    //echoMode: echo ? TextInput.Password : TextInput.Normal
                    echoMode: TextInput.Password
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
                onClicked: authenticationDialog.rejected()
            }

            Button {
                text: qsTr("Authenticate")
                onClicked: authenticationDialog.accepted()
            }

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
        }
    }
}
