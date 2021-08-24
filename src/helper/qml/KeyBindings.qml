// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtGSettings 1.0 as Settings
import Fluid.Core 1.0 as FluidCore
import Liri.WaylandClient 1.0 as WaylandClient

FluidCore.Object {
    Settings.GSettings {
        id: smKeybindings
        schema.id: "io.liri.desktop.keybindings.sm"
        schema.path: "/io/liri/desktop/keybindings/sm/"
    }

    Settings.GSettings {
        id: desktopKeybindings

        schema.id: "io.liri.desktop.keybindings.desktop"
        schema.path: "/io/liri/desktop/keybindings/desktop/"
    }

    /*
     * Session Manager
     */

    WaylandClient.LiriShortcut {
        sequence: smKeybindings.powerOff
        onActivated: {
            powerOffDialog.show();
        }
    }

    /*
     * Desktop
     */

    WaylandClient.LiriShortcut {
        sequence: desktopKeybindings.runCommand
        onActivated: {
            runDialog.show();
        }
    }
}
