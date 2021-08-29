// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml 2.1
import QtQuick 2.15
import QtQuick.Window 2.15
import QtGSettings 1.0 as Settings
import Liri.WaylandClient 1.0 as WaylandClient
import Liri.PolicyKit 1.0 as Polkit

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

    WaylandClient.LiriShell {
        id: shell

        onLogoutRequested: {
            logoutDialog.show();
        }
        onShutdownRequested: {
            powerOffDialog.show();
        }

        onActiveChanged: {
            if (active)
                readyTimer.start();
        }
    }

    WaylandClient.LiriLockScreenV1 {
        id: lockScreen

        onLockRequested: {
            lockScreenInstantiator.active = true;
        }
    }

    Component {
        id: osdComponent

        OsdWindow {}
    }

    WaylandClient.LiriOsd {
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

    NotificationsManager {}

    Instantiator {
        id: lockScreenInstantiator

        active: false
        model: Qt.application.screens

        LockScreenWindow {
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
