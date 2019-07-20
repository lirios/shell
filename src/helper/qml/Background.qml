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

import QtQuick 2.10
import QtQuick.Window 2.9
import QtQuick.Controls 2.1
import QtGSettings 1.0 as Settings
import Liri.WaylandClient 1.0 as WaylandClient
import Liri.Shell 1.0 as LS
import Liri.DBusService 1.0 as DBusService

Window {
    id: bgWindow

    flags: Qt.Window | Qt.BypassWindowManagerHint
    visible: false

    WaylandClient.WlrLayerSurfaceV1 {
        id: layerSurface

        shell: layerShell
        layer: WaylandClient.WlrLayerShellV1.BackgroundLayer
        window: bgWindow
        size.width: 0
        size.height: 0
        anchors: WaylandClient.WlrLayerSurfaceV1.TopAnchor |
                 WaylandClient.WlrLayerSurfaceV1.BottomAnchor |
                 WaylandClient.WlrLayerSurfaceV1.LeftAnchor |
                 WaylandClient.WlrLayerSurfaceV1.RightAnchor
        exclusiveZone: -1
        keyboardInteractivity: false
        nameSpace: "wallpaper"

        onConfigured: {
            bgWindow.width = width;
            bgWindow.height = height;
            console.debug("Configuring background to " + width + "x" + height);
            ackConfigure(serial);
            console.debug("Acked configure with serial", serial);
            bgWindow.visible = true;
        }
        onClosed: {
            this.destroy();
            bgWindow.close();
        }
    }

    Settings.GSettings {
        id: bgSettings

        schema.id: "io.liri.desktop.background"
        schema.path: "/io/liri/desktop/background/"
    }

    DBusService.Application {
        id: settingsApp

        serviceName: "io.liri.Settings"
        objectPath: "/io/liri/Settings"
    }

    Menu {
        id: bgMenu

        MenuItem {
            text: qsTr("Change Background...")
            onClicked: settingsApp.open(["background"])
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Display Settings")
            onClicked: settingsApp.open(["display"])
        }

        MenuItem {
            text: qsTr("Settings")
            onClicked: settingsApp.activate()
        }
    }

    LS.Background {
        anchors.fill: parent
        mode: bgSettings.mode
        pictureUrl: bgSettings.pictureUrl
        primaryColor: bgSettings.primaryColor
        secondaryColor: bgSettings.secondaryColor
        fillMode: bgSettings.fillMode
        blur: false
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: bgMenu.popup(mouse.x, mouse.y)
        onPressAndHold: bgMenu.popup(mouse.x, mouse.y)
    }
}
