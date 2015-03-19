/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

pragma Singleton

import QtQuick 2.0
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaii.misc 0.1 as Misc
import org.hawaii.settings 0.1 as Settings

Components.Object {
    readonly property alias background: bgSettings
    readonly property alias lockScreen: lockSettings

    /*
     * Keymap
     */

    Settings.ConfigGroup {
        id: keymapConfig
        file: "hawaii/keyboardrc"
        group: "Layout"
        onConfigChanged: applySettings()

        function applySettings() {
            compositor.settings.keyboardLayout = keymapConfig.readEntry("Layout1", "us");
            compositor.settings.keyboardVariant = keymapConfig.readEntry("Variant1");
            compositor.settings.keyboardOptions = keymapConfig.readEntry("Options");
            compositor.settings.keyboardRules = keymapConfig.readEntry("Rules", "evdev");
            compositor.settings.keyboardModel = keymapConfig.readEntry("Model", "pc105");
        }

        Component.onCompleted: keymapConfig.applySettings()
    }

    /*
     * Background
     */

    Settings.ConfigGroup {
        id: bgConfig
        file: "hawaii/shellrc"
        group: "Background"
        onConfigChanged: applySettings()

        function applySettings() {
            bgSettings.mode = bgConfig.readEntry("Mode", "wallpaper");
            bgSettings.primaryColor = bgConfig.readEntry("PrimaryColor", "#336699");
            bgSettings.secondaryColor = bgConfig.readEntry("SecondaryColor", "#334455");
            bgSettings.pictureUrl = bgConfig.readEntry("PictureUrl", Misc.StandardPaths.locateFile(Misc.StandardPaths.GenericDataLocation, "backgrounds/hawaii/Also_Calm.png"));
            bgSettings.fillMode = bgConfig.readEntry("FillMode", Image.Stretch);
        }
    }

    QtObject {
        id: bgSettings

        property string mode
        property color primaryColor
        property color secondaryColor
        property url pictureUrl
        property int fillMode

        Behavior on primaryColor {
            ColorAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.mediumDuration
            }
        }

        Behavior on secondaryColor {
            ColorAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.mediumDuration
            }
        }

        Component.onCompleted: bgConfig.applySettings()
    }

    /*
     * Lock screen
     */

    Settings.ConfigGroup {
        id: lockConfig
        file: "hawaii/shellrc"
        group: "LockScreen"
        onConfigChanged: applySettings()

        function applySettings() {
            lockSettings.mode = lockConfig.readEntry("Mode", "wallpaper");
            lockSettings.primaryColor = lockConfig.readEntry("PrimaryColor", "#336699");
            lockSettings.secondaryColor = lockConfig.readEntry("SecondaryColor", "#334455");
            lockSettings.pictureUrl = lockConfig.readEntry("PictureUrl", Misc.StandardPaths.locateFile(Misc.StandardPaths.GenericDataLocation, "backgrounds/hawaii/Also_Calm.png"));
            lockSettings.fillMode = lockConfig.readEntry("FillMode", Image.Stretch);
        }
    }

    QtObject {
        id: lockSettings

        property string mode
        property color primaryColor
        property color secondaryColor
        property url pictureUrl
        property int fillMode

        Behavior on primaryColor {
            ColorAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.mediumDuration
            }
        }

        Behavior on secondaryColor {
            ColorAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.mediumDuration
            }
        }

        Component.onCompleted: lockConfig.applySettings()
    }
}
