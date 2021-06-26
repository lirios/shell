// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import Fluid.Controls 1.0 as FluidControls

FluidControls.Card {
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
        NumberAnimation { duration: FluidControls.Units.shortDuration }
    }

    Behavior on height {
        NumberAnimation { duration: FluidControls.Units.shortDuration }
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
                margins: FluidControls.Units.largeSpacing
            }

            FluidControls.CircleImage {
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

                FluidControls.Icon {
                    anchors.centerIn: parent
                    source: FluidControls.Utils.iconUrl("action/account_circle")
                    size: 96
                }

                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                width: parent.width
                height: FluidControls.Units.largeSpacing
            }

            Label {
                id: label
                text: model.realName || model.name
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                maximumLineCount: 2
                font.pixelSize: 20

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            RowLayout {
                height: visible ? field.height : 0

                visible: opacity > 0.0
                opacity: selected ? 1.0 : 0.0

                spacing: FluidControls.Units.smallSpacing

                onVisibleChanged: {
                    if (!visible)
                        field.text = ""
                }

                Layout.fillWidth: true

                Behavior on height {
                    NumberAnimation { duration: FluidControls.Units.shortDuration }
                }

                Behavior on opacity {
                    NumberAnimation { duration: FluidControls.Units.shortDuration }
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

                ToolButton {
                    id: sendButton
                    icon.source: FluidControls.Utils.iconUrl("content/send")
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
                        NumberAnimation { duration: FluidControls.Units.shortDuration }
                    }

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                }
            }
        }
    }
}
