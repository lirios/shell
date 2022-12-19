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

import QtQuick 2.1
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import Fluid.Controls 1.0
import Fluid.Effects 1.0 as FluidEffects
import Liri.Shell 1.0
import Liri.LoginManager 1.0
import SddmComponents 2.0 as SddmComponents
import "indicators" as Indicators

ColumnLayout {
    id: greeter

    property bool primary: true

    property int selectedUser: userModel.lastIndex
    property int selectedSession: sessionIndicator.currentIndex

    signal userSelected(var user)

    signal loginRequested(string userName, string password, int sessionIndex)
    signal loginSucceeded()
    signal loginFailed(string message)

    property bool virtualKeyboardAvailable: false
    property bool virtualKeyboardActive: false
    signal virtualKeyboardToggleRequested()

    Connections {
        target: sddm
        onLoginSucceeded: {
            usersListView.currentItem.busyIndicator.opacity = 0.0
            usersListView.loginSucceeded()
            greeter.loginSucceeded()
        }
        onLoginFailed: {
            var message = qsTr("Sorry, wrong password. Please try again.")
            usersListView.currentItem.busyIndicator.opacity = 0.0
            usersListView.currentItem.field.text = ""
            usersListView.currentItem.field.forceActiveFocus()
            usersListView.loginFailed(message)
            greeter.loginFailed(message)
        }
    }

    DateTime {
        id: dateTime
    }

    Item {
        anchors.fill: parent

        LoginGreeter {
            id: usersListView
            anchors.centerIn: parent
            z: 1
            onCurrentIndexChanged: greeter.userSelected(currentItem)
            model: UsersModel {}
            onLoginRequested: greeter.loginRequested(userName, password, selectedSession)
        }

        Item {
            id: panel

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            height: 56

            RowLayout {
                anchors.fill: parent

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Item {
                    Layout.fillHeight: true
                    Layout.preferredWidth: indicatorsRect.visible ? indicatorsRect.implicitWidth + indicatorsRect.anchors.margins * 2 : 0

                    Rectangle {
                        id: indicatorsRect

                        anchors {
                            fill: parent
                            margins: Units.smallSpacing
                        }

                        implicitWidth: indicatorsView.width + 2 * Units.smallSpacing

                        radius: 2
                        color: Material.dialogColor
                        layer.enabled: visible
                        layer.effect: FluidEffects.Elevation {
                            elevation: 8
                        }

                        Row {
                            id: indicatorsView

                            anchors.centerIn: parent

                            height: parent.height

                            DateTimeIndicator {}

                            Indicators.PowerOff {
                                id: powerOffIndicator
                                visible: sddm.canSuspend || sddm.canHibernate || sddm.canPowerOff || sddm.canReboot
                            }

                            Indicators.Session {
                                id: sessionIndicator
                            }

                            Indicators.Keyboard {}

                            Indicators.VirtualKeyboard {}

                            // Enable once we moved the indicators to Liri.Desktop
                            /*
                            EventsIndicator {}

                            SettingsIndicator {}

                            SoundIndicator {}

                            NetworkIndicator {}

                            StorageIndicator {}

                            BatteryIndicator {}
                            */
                        }
                    }
                }
            }
        }
    }
}
