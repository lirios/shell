// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml 2.1
import QtQuick 2.15
import QtQuick.Window 2.15
import QtGSettings 1.0 as Settings
import Liri.WaylandClient 1.0 as WaylandClient
import "components" as Components
import "ui/notifications" as Notifications

Item {
    property int refCount: 0
    property Screen primaryScreen: null

    onRefCountChanged: {
        if (shell.active && refCount == 0)
            shell.sendReady();
    }

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

    WaylandClient.LiriShell {
        id: shell
    }

    WaylandClient.WlrLayerShellV1 {
        id: layerShell
    }

    Osd {}

    Instantiator {
        model: Qt.application.screens

        BackgroundWindow {
            screen: modelData
        }
    }

    Notifications.NotificationsManager {}

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
