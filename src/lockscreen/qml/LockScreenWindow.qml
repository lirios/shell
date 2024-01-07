// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtGSettings as Settings
import QtAccountsService as AccountsService
import Fluid.Controls as FluidControls
import Fluid.Effects as FluidEffects
import Liri.Shell
import Liri.Shell.LockScreen
import Liri.QtShellIntegration

Window {
    id: lockScreenWindow

    SessionLockSurfaceV1 {
        id: lockSurface
    }

    color: lockSettings.primaryColor
    visible: true

    Settings.GSettings {
        id: lockSettings

        schema.id: "io.liri.desktop.lockscreen"
        schema.path: "/io/liri/desktop/lockscreen/"
    }

    Loader {
        anchors.fill: parent
        asynchronous: false
        sourceComponent: {
            switch (lockSettings.mode) {
            case "hgradient":
            case "vgradient":
                return gradient
            case "wallpaper":
                return wallpaper
            default:
                break
            }
            return null
        }
    }

    Component {
        id: gradient

        Rectangle {
            property bool vertical: lockSettings.mode === "vgradient"

            rotation: vertical ? 270 : 0
            scale: vertical ? 2 : 1
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: lockSettings.primaryColor
                }
                GradientStop {
                    position: 1
                    color: lockSettings.secondaryColor
                }
            }
        }
    }

    Component {
        id: wallpaper

        Item {
            Image {
                id: image

                anchors.fill: parent
                source: lockSettings.pictureUrl
                sourceSize.width: lockScreenWindow.width
                sourceSize.height: lockScreenWindow.height
                fillMode: convertFillMode(lockSettings.fillMode)
                visible: false

                function convertFillMode(fillMode) {
                    switch (fillMode) {
                    case "preserve-aspect-fit":
                        return Image.PreserveAspectFit;
                    case "preserve-aspect-crop":
                        return Image.PreserveAspectCrop;
                    case "tile":
                        return Image.Tile;
                    case "tile-vertically":
                        return Image.TileVertically;
                    case "tile-horizontally":
                        return Image.TileHorizontally;
                    case "pad":
                        return Image.Pad;
                    default:
                        return Image.Stretch;
                    }
                }
            }

            FluidEffects.Vignette {
                anchors.fill: parent
                source: image
                radius: 4
                brightness: 0.4
            }
        }
    }

    LoginGreeter {
        id: usersListView

        Material.theme: Material.Dark
        Material.primary: Material.Blue
        Material.accent: Material.Blue

        anchors.centerIn: parent

        AccountsService.UserAccount {
            id: currentUser
        }

        model: ListModel {
            id: usersModel

            Component.onCompleted: {
                // ListElement cannot use script for property value, so we
                // have to append the element here
                usersModel.append({realName: currentUser.realName,
                                      userName: currentUser.userName,
                                      iconFileName: currentUser.iconFileName});
            }
        }
        onLoginRequested: {
            Authenticator.authenticate(password, function(succeeded) {
                if (succeeded) {
                    usersListView.currentItem.busyIndicator.opacity = 0.0;
                    usersListView.loginSucceeded();
                    lockSurface.unlockRequested();
                } else {
                    usersListView.currentItem.busyIndicator.opacity = 0.0;
                    usersListView.currentItem.field.text = "";
                    usersListView.currentItem.field.forceActiveFocus();
                    usersListView.loginFailed(qsTr("Sorry, wrong password. Please try again."));
                }
            });
        }
        onLoginFailed: {
            errorBar.open(message);
        }
    }

    FluidControls.SnackBar {
        id: errorBar
    }
}
