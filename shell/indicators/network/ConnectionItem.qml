/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 * Copyright (C) 2013-2014 Jan Grulich
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *    Jan Grulich <jgrulich@redhat.com>
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
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import Fluid.Controls 1.0
import org.hawaiios.misc 0.1 as Misc
import Hawaii.NetworkManager 1.0 as NM
import "../../components" as CustomComponents

Item {
    property bool predictableWirelessPassword: !Uuid && Type === NM.Enums.Wireless &&
                                               (SecurityType === NM.Enums.StaticWep || SecurityType === NM.Enums.WpaPsk ||
                                                SecurityType === NM.Enums.Wpa2Psk)
    property bool showSpeed: ConnectionState === NM.Enums.Activated &&
                             (Type === NM.Enums.Wimax ||
                              Type === NM.Enums.Wired ||
                              Type === NM.Enums.Wireless ||
                              Type === NM.Enums.Gsm ||
                              Type === NM.Enums.Cdma)
    property bool detailsVisible: false
    property bool passwordDialogVisible: false

    width: ListView.view.width
    height: mainLayout.implicitHeight
    states: [
        State {
            name: "collapsed"
            when: !(detailsVisible || passwordDialogVisible)

            StateChangeScript {
                script: {
                    if (expandableLoader.status === Loader.Ready)
                        expandableLoader.sourceComponent = undefined;
                }
            }
        },
        State {
            name: "expandedDetails"
            when: detailsVisible

            StateChangeScript {
                script: createContent()
            }
        },
        State {
            name: "expandedPasswordDialog"
            when: passwordDialogVisible

            StateChangeScript {
                script: createContent()
            }

            PropertyChanges {
                target: stateChangeButton
                opacity: 1.0
            }
        }
    ]

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            if (passwordDialogVisible)
                passwordDialogVisible = false;
            else
                detailsVisible = !detailsVisible;
        }
    }

    ColumnLayout {
        id: mainLayout
        spacing: Units.largeSpacing

        RowLayout {
            spacing: Units.smallSpacing

            Icon {
                name: indicator.massageIconName(ConnectionIcon)
                width: Units.iconSizes.medium
                height: width
                color: label.color

                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                spacing: Units.smallSpacing

                Label {
                    id: label
                    text: ItemUniqueName
                    elide: Text.ElideRight
                    font.weight: ConnectionState === NM.Enums.Activated ? Font.DemiBold : Font.Normal
                    font.italic: ConnectionState === NM.Enums.Activating ? true : false

                    Layout.fillWidth: true
                }

                Label {
                    id: statusLabel
                    text: {
                        if (ConnectionState === NM.Enums.Activating) {
                            if (Type === NM.Enums.Vpn)
                                return VpnState;
                            else
                                return DeviceState;
                        } else if (ConnectionState === NM.Enums.Deactivating) {
                            if (Type === NM.Enums.Vpn)
                                return VpnState;
                            else
                                return DeviceState;
                        } else if (ConnectionState === NM.Enums.Deactivated) {
                            var result = LastUsed;
                            if (SecurityType > NM.Enums.None)
                                result += ", " + SecurityTypeString;
                            return result;
                        } else if (ConnectionState === NM.Enums.Activated) {
                            return qsTr("Connected");
                        }
                    }
                    opacity: 0.6
                    elide: Text.ElideRight
                    font.pointSize: label.font.pointSize * 0.8

                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                }

                Loader {
                    id: expandableLoader
                    visible: status == Loader.Ready

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Layout.fillWidth: true
            }

            Item {
                Layout.fillWidth: true
            }

            CustomComponents.ToolButton {
                id: stateChangeButton
                iconName: ConnectionState === NM.Enums.Deactivated ? "network-connect" : "network-disconnect"
                //opacity: mouseArea.containsMouse ? 1.0 : 0.0
                //tooltip: ConnectionState === NM.Enums.Deactivated ? qsTr("Connect") : qsTr("Disconnect")
                visible: opacity == 1.0
                onClicked: changeState()

                Behavior on opacity {
                    NumberAnimation {
                        easing.type: Easing.InOutQuad
                        duration: Units.shortDuration
                    }
                }

                Layout.alignment: Qt.AlignTop
            }
        }
    }

    Component {
        id: detailsComponent

        ColumnLayout {
            spacing: Units.smallSpacing

            Repeater {
                model: ConnectionDetails.length / 2

                RowLayout {
                    spacing: Units.smallSpacing

                    Label {
                        text: ConnectionDetails[index * 2]
                        font.weight: Font.Bold
                        opacity: 0.6
                    }

                    Label {
                        text: ConnectionDetails[(index * 2) + 1]
                        textFormat: Text.StyledText
                        opacity: 0.6
                    }
                }
            }
        }
    }

    Component {
        id: passwordDialogComponent

        ColumnLayout {
            property alias passwordField: passwordField

            spacing: Units.smallSpacing

            TextField {
                id: passwordField
                echoMode: showPasswordCheckbox.checked ? TextInput.Normal : TextInput.Password
                placeholderText: qsTr("Password")
                validator: RegExpValidator {
                    regExp: {
                        if (SecurityType === NM.Enums.StaticWep)
                            /^(?:[\x20-\x7F]{5}|[0-9a-fA-F]{10}|[\x20-\x7F]{13}|[0-9a-fA-F]{26}){1}$/
                                                                                                   else
                                                                                                   /^(?:[\x20-\x7F]{8,64}){1}$/
                    }
                }
                onAccepted: changeState()
                onAcceptableInputChanged: stateChangeButton.enabled = acceptableInput

                Layout.fillWidth: true
            }

            CheckBox {
                id: showPasswordCheckbox
                checked: false
                text: qsTr("Show password")

                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    function createContent() {
        if (detailsVisible) {
            expandableLoader.sourceComponent = detailsComponent;
        } else if (passwordDialogVisible) {
            expandableLoader.sourceComponent = passwordDialogComponent;
            expandableLoader.item.passwordField.forceActiveFocus();
        }
    }

    function changeState() {
        detailsVisible = false;

        if (Uuid || !predictableWirelessPassword || passwordDialogVisible) {
            if (ConnectionState === NM.Enums.Deactivated) {
                if (!predictableWirelessPassword && !Uuid) {
                    handler.addAndActivateConnection(DevicePath, SpecificPath);
                } else if (passwordDialogVisible) {
                    if (expandableLoader.item.password !== "") {
                        handler.addAndActivateConnection(DevicePath, SpecificPath, expandableLoader.item.password);
                        passwordDialogVisible = false;
                    } else {
                        connectionItem.clicked();
                    }
                } else {
                    handler.activateConnection(ConnectionPath, DevicePath, SpecificPath);
                }
            } else {
                handler.deactivateConnection(ConnectionPath, DevicePath);
            }
        } else if (predictableWirelessPassword) {
            passwordDialogVisible = true;
        }
    }
}
