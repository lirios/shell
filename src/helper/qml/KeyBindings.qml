// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtGSettings 1.0 as Settings
import Fluid.Core 1.0 as FluidCore
import Liri.Shell 1.0 as Shell

FluidCore.Object {
    Settings.GSettings {
        id: wmKeybindings

        schema.id: "io.liri.desktop.keybindings.wm"
        schema.path: "/io/liri/desktop/keybindings/wm/"
    }

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
     * Window Manager
     */

    Shell.LiriShortcutV1 {
        sequence: wmKeybindings.mainMenu
        onActivated: {
            panelWindow;
        }
    }

    /*
     * Session Manager
     */

    Shell.LiriShortcutV1 {
        sequence: smKeybindings.powerOff
        onActivated: {
            powerOffDialog.show();
        }
    }

    /*
     * Desktop
     */

    Shell.LiriShortcutV1 {
        sequence: desktopKeybindings.runCommand
        onActivated: {
            runDialog.show();
        }
    }
}
