/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

        onActiveChanged: {
            if (active) {
                registerGrabSurface(grabWindow);
                grabWindow.visible = true;
            }
        }
    }

    WaylandClient.WlrLayerShellV1 {
        id: layerShell
    }

    Osd {}

    Components.BorderlessWindow {
        id: grabWindow

        width: 1
        height: 1
    }

    BackgroundWindow {}

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
