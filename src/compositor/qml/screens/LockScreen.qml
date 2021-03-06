// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0
import Fluid.Effects 1.0
import QtAccountsService 1.0
import Liri.Shell 1.0
import Liri.private.shell 1.0 as P
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
        ScriptAction { script: output.locked = true }
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
        ScriptAction { script: output.locked = false }
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
                    SessionInterface.authenticate(password, function(succeded) {
                        if (succeded) {
                            usersListView.currentItem.busyIndicator.opacity = 0.0
                            usersListView.loginSucceeded()
                            SessionInterface.unlock();
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
            }
        }
    }

    SnackBar {
        id: errorBar
    }
}
