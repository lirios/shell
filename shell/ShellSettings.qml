/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import GreenIsland 1.0 as GreenIsland
import Hawaii.Settings 1.0 as Settings
import Fluid.Core 1.0 as FluidCore

FluidCore.Object {
    readonly property alias keyboard: keyboardSettings
    readonly property alias background: bgSettings
    readonly property alias lockScreen: lockSettings
    readonly property alias session: sessionSettings
    readonly property int numWorkspaces: 4

    id: shellSettings

    /*
     * Keymap
     */

    GreenIsland.CompositorSettings {
        id: compositorSettings
        compositor: compositor
        keymap: GreenIsland.Keymap {
            layout: keyboardSettings.layouts[0] ? keyboardSettings.layouts[0] : "us"
            variant: keyboardSettings.variants[0] ? keyboardSettings.variants[0] : ""
            options: keyboardSettings.options[0] ? keyboardSettings.options[0] : ""
            model: keyboardSettings.model
            rules: keyboardSettings.rules[0] ? keyboardSettings.rules[0] : ""
        }
    }

    Settings.Settings {
        id: keyboardSettings
        schema.id: "org.hawaiios.desktop.peripherals.keyboard"
        schema.path: "/org/hawaiios/desktop/peripherals/keyboard/"
        onSettingsChanged: shellSettings.applyKeymapSettings()
    }

    /*
     * Session
     */

    Settings.Settings {
        id: sessionSettings
        schema.id: "org.hawaiios.session"
        schema.path: "/org/hawaiios/session/"
    }

    /*
     * Background
     */

    Settings.Settings {
        id: bgSettings
        schema.id: "org.hawaiios.desktop.background"
        schema.path: "/org/hawaiios/desktop/background/"
    }

    /*
     * Lock screen
     */

    Settings.Settings {
        id: lockSettings
        schema.id: "org.hawaiios.desktop.lockscreen"
        schema.path: "/org/hawaiios/desktop/lockscreen/"
    }

    /*
     * Methods
     */

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
