/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013-2014 Jan Grulich <jgrulich@redhat.com>
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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.kde.kcoreaddons 1.0 as KCoreAddons
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Item {
    property bool predictableWirelessPassword: !Uuid && Type === PlasmaNM.Enums.Wireless &&
                                               (SecurityType === PlasmaNM.Enums.StaticWep || SecurityType === PlasmaNM.Enums.WpaPsk ||
                                                SecurityType === PlasmaNM.Enums.Wpa2Psk)
    property bool showSpeed: ConnectionState === PlasmaNM.Enums.Activated &&
                             (Type === PlasmaNM.Enums.Wimax ||
                              Type === PlasmaNM.Enums.Wired ||
                              Type === PlasmaNM.Enums.Wireless ||
                              Type === PlasmaNM.Enums.Gsm ||
                              Type === PlasmaNM.Enums.Cdma)
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

    PlasmaCore.DataSource {
        property string downloadSource: "network/interfaces/" + DeviceName + "/receiver/data"
        property string uploadSource: "network/interfaces/" + DeviceName + "/transmitter/data"

        id: dataSource
        engine: "systemmonitor"
        connectedSources: showSpeed && indicator.expanded ? [downloadSource, uploadSource] : []
        interval: 2000
    }

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
        spacing: Themes.Units.largeSpacing

        RowLayout {
            spacing: Themes.Units.smallSpacing

            Components.Icon {
                iconName: indicator.massageIconName(ConnectionIcon)
                width: Themes.Units.iconSizes.medium
                height: width
                color: Themes.Theme.palette.panel.textColor

                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                spacing: Themes.Units.smallSpacing

                Label {
                    id: label
                    text: ItemUniqueName
                    elide: Text.ElideRight
                    font.weight: ConnectionState === PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
                    font.italic: ConnectionState === PlasmaNM.Enums.Activating ? true : false
                    color: Themes.Theme.palette.panel.textColor

                    Layout.fillWidth: true
                }

                Label {
                    id: statusLabel
                    text: {
                        if (ConnectionState === PlasmaNM.Enums.Activating) {
                            if (Type === PlasmaNM.Enums.Vpn)
                                return VpnState;
                            else
                                return DeviceState;
                        } else if (ConnectionState === PlasmaNM.Enums.Deactivating) {
                            if (Type === PlasmaNM.Enums.Vpn)
                                return VpnState;
                            else
                                return DeviceState;
                        } else if (ConnectionState === PlasmaNM.Enums.Deactivated) {
                            var result = LastUsed;
                            if (SecurityType > PlasmaNM.Enums.None)
                                result += ", " + SecurityTypeString;
                            return result;
                        } else if (ConnectionState === PlasmaNM.Enums.Activated) {
                            if (showSpeed && dataSource.data && dataSource.data[dataSource.downloadSource] && dataSource.data[dataSource.uploadSource]) {
                                var arg1 = KCoreAddons.Format.formatByteSize(dataSource.data[dataSource.downloadSource].value * 1024 || 0);
                                var arg2 = KCoreAddons.Format.formatByteSize(dataSource.data[dataSource.uploadSource].value * 1024 || 0)
                                return qsTr("Connected, ⬇ %1/s, ⬆ %2/s").arg(arg1).arg(arg2);
                            } else {
                                return qsTr("Connected");
                            }
                        }
                    }
                    opacity: 0.6
                    elide: Text.ElideRight
                    font.pointSize: label.font.pointSize * 0.8
                    color: Themes.Theme.palette.panel.textColor

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

            ToolButton {
                id: stateChangeButton
                iconName: ConnectionState === PlasmaNM.Enums.Deactivated ? "network-connect" : "network-disconnect"
                //opacity: mouseArea.containsMouse ? 1.0 : 0.0
                tooltip: ConnectionState === PlasmaNM.Enums.Deactivated ? qsTr("Connect") : qsTr("Disconnect")
                visible: opacity == 1.0
                onClicked: changeState()

                Behavior on opacity {
                    NumberAnimation {
                        easing.type: Easing.InOutQuad
                        duration: Themes.Units.shortDuration
                    }
                }

                Layout.alignment: Qt.AlignTop
            }
        }
    }

    Component {
        id: detailsComponent

        ColumnLayout {
            spacing: Themes.Units.smallSpacing

            Repeater {
                model: ConnectionDetails.length / 2

                RowLayout {
                    spacing: Themes.Units.smallSpacing

                    Label {
                        text: ConnectionDetails[index * 2]
                        color: Themes.Theme.palette.panel.textColor
                        font.weight: Font.Bold
                        opacity: 0.6
                    }

                    Label {
                        text: ConnectionDetails[(index * 2) + 1]
                        color: Themes.Theme.palette.panel.textColor
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

            spacing: Themes.Units.smallSpacing

            TextField {
                id: passwordField
                echoMode: showPasswordCheckbox.checked ? TextInput.Normal : TextInput.Password
                placeholderText: qsTr("Password")
                validator: RegExpValidator {
                    regExp: {
                        if (SecurityType === PlasmaNM.Enums.StaticWep)
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
            if (ConnectionState === PlasmaNM.Enums.Deactivated) {
                if (!predictableWirelessPassword && !Uuid) {
                    handler.addAndActivateConnection(DevicePath, SpecificPath);
                } else if (passwordDialogVisible) {
                    if (expandableComponentLoader.item.password !== "") {
                        handler.addAndActivateConnection(DevicePath, SpecificPath, expandableComponentLoader.item.password);
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
