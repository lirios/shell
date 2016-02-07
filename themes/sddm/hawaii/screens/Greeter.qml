/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import SddmComponents 2.0 as SddmComponents
import Hawaii.Themes 1.0 as Themes
import "../components" as Components

FocusScope {
    property int sessionIndex: sessionModel.lastIndex
    property bool rebootVisible: true
    property bool powerOffVisible: true

    signal loginRequested(string userName, string password, int sessionIndex)
    signal rebootRequested()
    signal powerOffRequested()

    id: greeter

    Rectangle {
        id: stripe
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        color: "#80000000"
        height: usersView.itemSize
    }

    ListView {
        property real faceSize: Themes.Units.iconSizes.huge
        property real itemSize: faceSize * 1.8

        id: usersView
        anchors {
            left: parent.left
            top: stripe.top
            right: parent.right
            leftMargin: Themes.Units.dp(50)
        }
        height: itemSize * 2
        z: 1
        highlightRangeMode: ListView.StrictlyEnforceRange
        interactive: true
        focus: true
        model: userModel
        currentIndex: model.lastIndex
        spacing: Themes.Units.dp(40)
        delegate: ColumnLayout {
            Item { height: usersView.faceSize * 0.1 }

            Row {
                spacing: Themes.Units.largeSpacing

                Components.CircleImage {
                    id: image
                    iconSize: usersView.faceSize
                    iconSource: model.icon
                    visible: status == Image.Ready
                }

                Components.CircleIcon {
                    id: icon
                    iconSize: usersView.faceSize
                    iconName: "avatar-default-symbolic"
                    visible: !image.visible
                }

                Column {
                    spacing: Themes.Units.smallSpacing
                    anchors.verticalCenter: image.visible ? image.verticalCenter : icon.verticalCenter

                    Text {
                        renderType: Text.NativeRendering
                        color: "white"
                        style: Text.Raised
                        styleColor: "black"
                        text: (model.realName === "") ? model.name : model.realName
                        font.family: "Noto Sans"
                        font.pointSize: 16
                    }

                    Components.PasswordField {
                        id: passwordField
                        width: Themes.Units.dp(250)
                        focus: visible
                        visible: usersView.currentIndex == index
                        onAccepted: greeter.loginRequested(model.name, text, indicators.selectedSessionIndex)
                        onVisibleChanged: if (visible) forceActiveFocus()

                        // WORKAROUND: Focus works with qmlscene, however it's broken
                        // with SDDM. To make it work we use the following hack.
                        Timer {
                            interval: 200
                            running: true
                            onTriggered: passwordField.forceActiveFocus()
                        }
                        // END WORKAROUND
                    }

                    Connections {
                        target: sddm
                        onLoginFailed: {
                            messageBox.setErrorMessage(qsTr("Login Failed"));
                            passwordField.selectAll();
                            passwordField.forceActiveFocus();
                        }
                    }

                    Components.MessageBox {
                        id: messageBox
                        visible: passwordField.visible
                    }
                }
            }
        }
    }

    Components.Indicators {
        id: indicators
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        rebootVisible: greeter.rebootVisible
        powerOffVisible: greeter.powerOffVisible
        onRebootRequested: greeter.rebootRequested()
        onPowerOffRequested: greeter.powerOffRequested()
    }
}
