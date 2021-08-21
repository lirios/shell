// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtGSettings 1.0 as Settings
import QtAccountsService 1.0 as AccountsService
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects
import Liri.WaylandClient 1.0 as WaylandClient
import Liri.Shell 1.0
import Liri.ShellHelper 1.0 as ShellHelper

Window {
    visible: true

    Settings.GSettings {
        id: lockSettings

        schema.id: "io.liri.desktop.lockscreen"
        schema.path: "/io/liri/desktop/lockscreen/"
    }

    WaylandClient.WlrLayerSurfaceV1 {
        layer: WaylandClient.WlrLayerSurfaceV1.OverlayLayer
        anchors: WaylandClient.WlrLayerSurfaceV1.TopAnchor |
                 WaylandClient.WlrLayerSurfaceV1.BottomAnchor |
                 WaylandClient.WlrLayerSurfaceV1.LeftAnchor |
                 WaylandClient.WlrLayerSurfaceV1.RightAnchor
        keyboardInteractivity: WaylandClient.WlrLayerSurfaceV1.ExclusiveKeyboardInteractivity
        exclusiveZone: -1
        role: "lockscreen"
    }

    Background {
        id: background

        anchors.fill: parent
        mode: lockSettings.mode
        pictureUrl: lockSettings.pictureUrl
        primaryColor: lockSettings.primaryColor
        secondaryColor: lockSettings.secondaryColor
        fillMode: lockSettings.fillMode
        blur: true
        visible: !vignette.visible
    }

    FluidEffects.Vignette {
        id: vignette

        anchors.fill: parent
        source: background
        radius: 4
        brightness: 0.4
        visible: background.imageLoaded
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
            ShellHelper.Authenticator.authenticate(password, function(succeeded) {
                if (succeeded) {
                    usersListView.currentItem.busyIndicator.opacity = 0.0;
                    usersListView.loginSucceeded();
                    lockScreen.unlock();
                    lockScreenInstantiator.active = false;
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
