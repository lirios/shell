/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2015 Michael Spencer <sonrisesoftware@gmail.com>
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

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import Fluid.Controls 1.0

Card {
    id: userDelegate

    property string userName: model.userName
    property string realName: model.realName
    property string iconFileName: model.iconFileName
    property string backgroundMode: model.backgroundMode
    property url pictureUrl: model.pictureUrl
    property string fillMode: model.fillMode
    property string primaryColor: model.primaryColor
    property string secondaryColor: model.secondaryColor

    property bool selected: ListView.isCurrentItem

    property alias field: field
    property alias busyIndicator: busyIndicator

    signal accepted(string password)

    anchors.verticalCenter: parent.verticalCenter
    width: selected ? itemSelectedSize.width : itemSize.width
    height: selected ? itemSelectedSize.height : itemSize.height

    Material.theme: Material.Light
    Material.elevation: selected ? 4 : 2

    Behavior on width {
        NumberAnimation { duration: Units.shortDuration }
    }

    Behavior on height {
        NumberAnimation { duration: Units.shortDuration }
    }

    onSelectedChanged: {
        // Make sure the password field is focused when
        // the user is selected
        if (selected)
            field.forceActiveFocus()
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        preventStealing: true
        propagateComposedEvents: true

        onPressed: {
            // Propagate the click to the list view, please note
            // that this has to be onPressed not onClicked
            mouse.accepted = false

            // Select this delegate
            usersListView.currentIndex = index
            usersListView.focus = true

            // Give focus to the field anyway, so that if we are
            // in the lock screen (which has only one delegate) and
            // the focus is lost for whatever reason the user
            // will be able to restore it with a click
            field.forceActiveFocus()
        }

        ColumnLayout {
            anchors {
                fill: parent
                margins: Units.largeSpacing
            }

            CircleImage {
                id: image

                width: 96
                height: width
                visible: status == Image.Ready && String(source).indexOf("sddm/faces/default.face.icon") == -1
                source: model.iconFileName

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: Qt.rgba(0, 0, 0, 0.3)
                    radius: width/2
                }

                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                width: image.width
                height: width
                visible: !image.visible

                Icon {
                    anchors.centerIn: parent
                    name: "action/account_circle"
                    size: 96
                }

                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                width: parent.width
                height: Units.largeSpacing
            }

            Label {
                id: label
                text: model.realName || model.name
                height: implicitHeight + Units.smallSpacing
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 20

                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                width: parent.width
                height: Units.largeSpacing
            }

            RowLayout {
                height: visible ? field.height : 0

                visible: opacity > 0.0
                opacity: selected ? 1.0 : 0.0

                spacing: Units.smallSpacing

                onVisibleChanged: {
                    if (!visible)
                        field.text = ""
                }

                Layout.fillWidth: true

                Behavior on height {
                    NumberAnimation { duration: Units.shortDuration }
                }

                Behavior on opacity {
                    NumberAnimation { duration: Units.shortDuration }
                }

                TextField {
                    id: field
                    width: parent.width
                    placeholderText: qsTr("Password")
                    echoMode: TextInput.Password
                    enabled: !busyIndicator.visible

                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    onAccepted: {
                        busyIndicator.opacity = 1.0
                        userDelegate.accepted(text)
                    }
                }

                IconButton {
                    id: sendButton
                    iconName: "content/send"
                    enabled: field.text != ""
                    onClicked: field.accepted()
                    visible: !busyIndicator.visible

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                }

                BusyIndicator {
                    id: busyIndicator
                    implicitWidth: sendButton.width
                    implicitHeight: sendButton.height
                    width: implicitWidth
                    height: implicitHeight
                    opacity: 0.0
                    visible: opacity > 0.0

                    Behavior on opacity {
                        NumberAnimation { duration: Units.shortDuration }
                    }

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                }
            }
        }
    }
}
