/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.4
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Effects 1.0
import QtAccountsService 1.0
import Liri.Shell 1.0
import "../indicators"

Showable {
    id: root

    property alias primary: loader.active

    clip: true
    y: -root.height
    visible: true

    showAnimation: SequentialAnimation {
        NumberAnimation {
            target: root
            property: "y"
            easing.type: Easing.OutQuad
            duration: Units.longDuration
            from: -root.height
            to: 0
        }
        ScriptAction { script: output.screenView.locked = true }
    }

    hideAnimation: SequentialAnimation {
        NumberAnimation {
            target: root
            property: "y"
            easing.type: Easing.OutQuad
            duration: Units.longDuration
            from: 0
            to: -root.height
        }
        ScriptAction { script: output.screenView.locked = false }
    }

    Material.theme: Material.Dark
    Material.primary: Material.color(Material.Blue, Material.Shade500)
    Material.accent: Material.color(Material.Blue, Material.Shade500)

    // Prevent mouse input from leaking through the lock screen
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        onPressed: {
            usersListView.currentItem.field.forceActiveFocus()
            mouse.accept = false
        }
    }

    Background {
        id: background
        anchors.fill: parent
        mode: liriCompositor.settings.lockScreen.mode
        pictureUrl: liriCompositor.settings.lockScreen.pictureUrl
        primaryColor: liriCompositor.settings.lockScreen.primaryColor
        secondaryColor: liriCompositor.settings.lockScreen.secondaryColor
        fillMode: liriCompositor.settings.lockScreen.fillMode
        blur: true
        visible: !vignette.visible
    }

    Vignette {
        id: vignette
        anchors.fill: parent
        source: background
        radius: 4
        brightness: 0.4
        visible: background.mode === "wallpaper" && background.imageLoaded
    }

    Item {
        anchors.fill: parent

        Loader {
            id: loader
            anchors.centerIn: parent
            active: primary
            z: 1
            sourceComponent: LoginGreeter {
                id: usersListView

                UserAccount {
                    id: currentUser
                }

                model: ListModel {
                    id: usersModel

                    Component.onCompleted: {
                        usersModel.append({
                                              "realName": currentUser.realName,
                                              "userName": currentUser.userName,
                                              "iconFileName": currentUser.iconFileName
                                          })
                    }
                }
                onLoginRequested: {
                    SessionInterface.unlockSession(password, function(succeded) {
                        if (succeded) {
                            usersListView.currentItem.busyIndicator.opacity = 0.0
                            usersListView.loginSucceeded()
                        } else {
                            usersListView.currentItem.busyIndicator.opacity = 0.0
                            usersListView.currentItem.field.text = ""
                            usersListView.currentItem.field.forceActiveFocus()
                            usersListView.loginFailed(qsTr("Sorry, wrong password. Please try again."))
                        }
                    })
                }
                onLoginFailed: errorBar.open(message)
            }
        }

        Item {
            id: panel

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                bottomMargin: root.y
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
                    Layout.preferredWidth: indicatorsRect.implicitWidth + indicatorsRect.anchors.margins * 2

                    Rectangle {
                        id: indicatorsRect

                        anchors {
                            fill: parent
                            margins: Units.smallSpacing
                        }

                        implicitWidth: indicatorsView.width + 2 * Units.smallSpacing

                        radius: 2
                        color: Material.dialogColor
                        layer.enabled: true
                        layer.effect: Elevation {
                            elevation: 8
                        }

                        Row {
                            id: indicatorsView

                            anchors.centerIn: parent

                            height: parent.height

                            DateTimeIndicator {}

                            NotificationsIndicator {}

                            SoundIndicator {}

                            NetworkIndicator {}

                            StorageIndicator {}

                            BatteryIndicator {}
                        }
                    }
                }
            }
        }
    }

    InfoBar {
        id: errorBar
    }
}
