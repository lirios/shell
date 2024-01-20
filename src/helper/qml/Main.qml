// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml
import QtQuick
import QtQuick.Window
import QtGSettings as Settings
import Liri.Shell as Shell
import Liri.Session as Session
import Liri.PolicyKit as Polkit

Item {
    property Screen primaryScreen: null

    Component.onCompleted: {
        primaryScreen = determinePrimaryScreen();
    }

    Timer {
        id: readyTimer

        interval: 250
        onTriggered: {
            shell.sendReady();
            topLayerWindow.show();
        }
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

    Shell.LiriShellV1 {
        id: shell

        onActiveChanged: {
            if (active)
                readyTimer.start();
        }
    }

    TopLayerWindow {
        id: topLayerWindow

        screen: primaryScreen

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

    Component {
        id: osdComponent

        OsdWindow {}
    }

    Shell.LiriOsdV1 {
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

    Instantiator {
        model: Qt.application.screens

        BackgroundWindow {
            screen: modelData
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
