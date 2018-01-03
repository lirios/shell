/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Fluid.Controls 1.0 as FluidControls

Dialog {
    id: authDialog

    property string actionId
    property alias message: messageLabel.text
    property string iconName
    property alias realName: avatarName.text
    property string avatar
    property alias prompt: promptLabel.text
    property bool echo: false
    property alias infoMessage: infoLabel.text
    property alias errorMessage: errorLabel.text

    parent: screenView

    title: qsTr("Authentication required")

    modal: true

    // Since we cannot determine when the dialog is canceled
    // with press outside or escape we shall not have autoclose
    closePolicy: Popup.NoAutoClose

    onAvatarChanged: {
        // Load the image from the disk if it's an absolute path
        if (avatar.indexOf("/") == 0) {
            avatarImage.name = ""
            avatarImage.source = avatar
        }

        // Otherwise use a standard icon
        avatarImage.source = ""
        avatarImage.name = "action/verified_user"
    }
    onErrorMessageChanged: {
        // Give focus to the password field and clear
        passwordInput.text = ""
        passwordInput.forceActiveFocus()
    }
    onOpened: {
        // Give focus to the password field
        passwordInput.forceActiveFocus()
    }
    onClosed: {
        // Cleanup
        actionId = ""
        message = ""
        iconName = ""
        realName = ""
        avatar = ""
        prompt = ""
        passwordInput.text = ""
        infoMessage = ""
        errorMessage = ""
    }

    Material.theme: Material.Dark
    Material.accent: Material.Blue

    ColumnLayout {
        id: mainLayout
        spacing: FluidControls.Units.smallSpacing

        RowLayout {
            spacing: FluidControls.Units.smallSpacing

            FluidControls.Icon {
                source: FluidControls.Utils.iconUrl("action/lock")
                size: FluidControls.Units.iconSizes.medium

                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                spacing: FluidControls.Units.smallSpacing

                FluidControls.DialogLabel {
                    id: messageLabel
                    wrapMode: Text.WordWrap

                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: FluidControls.Units.smallSpacing

                    FluidControls.Icon {
                        id: avatarImage
                        size: FluidControls.Units.iconSizes.large
                    }

                    Label {
                        id: avatarName

                        Layout.alignment: Qt.AlignVCenter
                    }

                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: FluidControls.Units.smallSpacing

                    Label {
                        id: promptLabel
                    }

                    TextField {
                        id: passwordInput
                        echoMode: echo ? TextInput.Normal : TextInput.Password
                        focus: true
                        onAccepted: authDialog.accepted()

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
    }

    footer: DialogButtonBox {
        Button {
            text: qsTr("Authenticate")
            flat: true

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }

        Button {
            text: qsTr("Cancel")
            flat: true

            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    onAccepted: policyKitAgent.authenticate(passwordInput.text)
    onRejected: policyKitAgent.abortAuthentication()
}
