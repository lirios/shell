/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
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
import QtWayland.Compositor 1.0
import QtGSettings 1.0 as Settings
import Fluid.Core 1.0 as FluidCore
import Liri.private.shell 1.0 as P

FluidCore.Object {
    readonly property alias ui: interfaceSettings
    readonly property alias keyboard: keyboardSettings
    readonly property alias background: bgSettings
    readonly property alias lockScreen: lockSettings
    readonly property alias shell: shellSettings
    readonly property alias session: sessionSettings
    readonly property int numWorkspaces: 4

    /*
     * Interface
     */

    Settings.GSettings {
        id: interfaceSettings
        schema.id: "io.liri.desktop.interface"
        schema.path: "/io/liri/desktop/interface/"
    }

    /*
     * Keymap
     */

    P.InputSettings {
        compositor: liriCompositor
    }

    Settings.GSettings {
        id: keyboardSettings
        schema.id: "io.liri.desktop.peripherals.keyboard"
        schema.path: "/io/liri/desktop/peripherals/keyboard/"
    }

    /*
     * Shell
     */

    Settings.GSettings {
        id: shellSettings
        schema.id: "io.liri.shell"
        schema.path: "/io/liri/shell/"
    }

    /*
     * Session
     */

    Settings.GSettings {
        id: sessionSettings
        schema.id: "io.liri.session"
        schema.path: "/io/liri/session/"
    }

    /*
     * Background
     */

    Settings.GSettings {
        id: bgSettings
        schema.id: "io.liri.desktop.background"
        schema.path: "/io/liri/desktop/background/"
    }

    /*
     * Lock screen
     */

    Settings.GSettings {
        id: lockSettings
        schema.id: "io.liri.desktop.lockscreen"
        schema.path: "/io/liri/desktop/lockscreen/"
    }
}
