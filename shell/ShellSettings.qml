/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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

pragma Singleton

import QtQuick 2.0
import GreenIsland 1.0 as GreenIsland
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaii.settings 0.2 as Settings

Components.Object {
    readonly property alias keyboard: keyboardSettings
    readonly property alias background: bgSettings
    readonly property alias lockScreen: lockSettings

    id: root

    /*
     * Keymap
     */

    Settings.Settings {
        id: keyboardSettings
        schema.id: "org.hawaii.desktop.peripherals.keyboard"
        schema.path: "/org/hawaii/desktop/peripherals/keyboard/"
        onSettingsChanged: root.applyKeymapSettings()
    }

    /*
     * Background
     */

    Settings.Settings {
        id: bgSettings
        schema.id: "org.hawaii.desktop.background"
        schema.path: "/org/hawaii/desktop/background/"
    }

    /*
     * Lock screen
     */

    Settings.Settings {
        id: lockSettings
        schema.id: "org.hawaii.desktop.lockscreen"
        schema.path: "/org/hawaii/desktop/lockscreen/"
    }

    /*
     * Key bindings
     */

    Settings.Settings {
        id: wmKeybindings
        schema.id: "org.hawaii.desktop.keybindings.wm"
        schema.path: "/org/hawaii/desktop/keybindings/wm/"
        onSettingsChanged: root.applyKeyBindings(wmKeybindings)
    }

    Settings.Settings {
        id: smKeybindings
        schema.id: "org.hawaii.desktop.keybindings.sm"
        schema.path: "/org/hawaii/desktop/keybindings/sm/"
        onSettingsChanged: root.applyKeyBindings(smKeybindings)
    }

    Settings.Settings {
        id: mmKeybindings
        schema.id: "org.hawaii.desktop.keybindings.multimedia"
        schema.path: "/org/hawaii/desktop/keybindings/multimedia/"
        onSettingsChanged: root.applyKeyBindings(mmKeybindings)
    }

    /*
     * Methods
     */

    function applyKeymapSettings() {
        if (keyboardSettings.layouts[0])
            GreenIsland.Compositor.settings.keyboardLayout = keyboardSettings.layouts[0];
        if (keyboardSettings.variants[0])
            GreenIsland.Compositor.settings.keyboardVariant = keyboardSettings.variants[0];
        if (keyboardSettings.options[0])
            GreenIsland.Compositor.settings.keyboardOptions = keyboardSettings.options[0];
        if (keyboardSettings.rules[0])
            GreenIsland.Compositor.settings.keyboardRules = keyboardSettings.rules[0];
        if (keyboardSettings.model)
            GreenIsland.Compositor.settings.keyboardModel = keyboardSettings.model;
    }

    function applyKeyBindings(o) {
        var i, j;
        for (i = 0; i < o.schema.keys.length; i++) {
            var name = o.schema.keys[i];
            var bindings = o[name];

            for (j = 0; j < bindings.length; j++)
                GreenIsland.KeyBindings.registerKeyBinding(name, bindings[j]);
            if (bindings.length === 0)
                GreenIsland.KeyBindings.unregisterKeyBinding(name);
        }
    }

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

    Component.onCompleted: {
        applyKeymapSettings();
        applyKeyBindings(wmKeybindings);
        applyKeyBindings(smKeybindings);
        applyKeyBindings(mmKeybindings);
    }
}
