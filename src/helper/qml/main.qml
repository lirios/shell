// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml 2.1
import QtQml.Models 2.15
import QtQuick 2.15
import QtQuick.Window 2.15
import QtGSettings 1.0 as Settings
import Fluid.Core 1.0 as FluidCore
import Aurora.Client 1.0 as AuroraClient
import Liri.Session 1.0 as Session
import Liri.PolicyKit 1.0 as Polkit

Item {
    property Screen primaryScreen: null

    Component.onCompleted: {
        primaryScreen = determinePrimaryScreen();
    }

    /*
     * Primary screen
     */

    Settings.GSettings {
        id: screenSettings

        schema.id: "io.liri.hardware.screens"
        schema.path: "/io/liri/hardware/screens/"
    }

    Connections {
        target: screenSettings

        function onPrimaryChanged() {
            primaryScreen = determinePrimaryScreen();
        }
    }

    /*
     * Key bindings
     */

    KeyBindings {}

    /*
     * Shell
     */

    AuroraClient.LiriShellV1 {
        id: shell
    }

    /*
    Instantiator {
        model: Qt.application.screens
        active: !workspaceReady

        SplashWindow {
            screen: modelData
        }
    }
    */

    Instantiator {
        model: Qt.application.screens
        active: shell.active

        BackgroundWindow {
            screen: modelData
        }
    }

    Instantiator {
        model: Qt.application.screens
        active: shell.active

        onObjectAdded: {
            shell.sendReady(object);
        }

        TopLayerWindow {
            screen: modelData

            onLogoutRequested: {
                logoutDialog.show();
            }
            onLockRequested: {
                Session.SessionManager.lock();
            }
            onShutdownRequested: {
                powerOffDialog.show();
            }
        }
    }

    Component {
        id: osdComponent

        OsdWindow {}
    }

    AuroraClient.LiriOsdV1 {
        property OsdWindow osdWindow: null

        onTextRequested: {
            if (!osdWindow)
                osdWindow = osdComponent.createObject(this);
            osdWindow.iconName = iconName;
            osdWindow.text = text;
            osdWindow.progressVisible = false;
            osdWindow.showWindow();
        }
        onProgressRequested: {
            if (!osdWindow)
                osdWindow = osdComponent.createObject(this);
            osdWindow.iconName = iconName;
            osdWindow.text = "";
            osdWindow.value = value;
            osdWindow.progressVisible = true;
            osdWindow.showWindow();
        }
    }

    /*
     * PolicyKit
     */

    AuthDialog {
        id: authDialog

        screen: null
    }

    Polkit.PolicyKitAgent {
        id: policyKitAgent

        onAuthenticationInitiated: {
            authDialog.actionId = actionId;
            authDialog.message = message;
            authDialog.iconName = iconName;
            authDialog.realName = realName;
        }
        onAuthenticationRequested: {
            authDialog.prompt = prompt;
            authDialog.echo = echo;
            authDialog.show();
        }
        onAuthenticationCanceled: {
            authDialog.hide();
        }
        onAuthenticationFinished: {
            authDialog.hide();
        }
        onAuthorizationGained: {
            authDialog.hide();
        }
        onAuthorizationFailed: {
            authDialog.errorMessage = qsTr("Sorry, that didn't work. Please try again.");
        }
        onAuthorizationCanceled: {
            authDialog.close();
        }
        onInformation: {
            authDialog.infoMessage = message;
        }
        onError: {
            authDialog.errorMessage = message;
        }
    }

    /*
     * Dialogs
     */

    RunDialog {
        id: runDialog

        screen: primaryScreen
    }

    LogoutDialog {
        id: logoutDialog

        screen: primaryScreen
    }

    PowerOffDialog {
        id: powerOffDialog

        screen: primaryScreen
    }

    /*
     * Methods
     */

    function determinePrimaryScreen() {
        for (var i = 0; i < Qt.application.screens.length; i++) {
            var screen = Qt.application.screens[i];
            if (screen.name === screenSettings.primary)
                return screen;
        }

        return null;
    }
}
