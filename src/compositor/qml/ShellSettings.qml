// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtWayland.Compositor 1.15
import QtGSettings 1.0 as Settings
import Fluid.Core 1.0 as FluidCore
import Liri.private.shell 1.0 as P

FluidCore.Object {
    readonly property alias outputs: outputsSettings
    readonly property alias ui: interfaceSettings
    readonly property alias keyboard: keyboardSettings
    readonly property alias shell: shellSettings
    readonly property alias session: sessionSettings
    readonly property int numWorkspaces: 4

    readonly property int windowActionModifier: shellSettings.windowActionKey === "Alt" ? Qt.AltModifier : Qt.MetaModifier

    /*
     * Outputs
     */

    Settings.GSettings {
        id: outputsSettings
        schema.id: "io.liri.hardware.screens"
        schema.path: "/io/liri/hardware/screens/"
    }

    Connections {
        target: outputsSettings

        function onPrimaryChanged() {
            for (var i = 0; i < liriCompositor.outputs.length; i++) {
                var output = liriCompositor.outputs[i];

                if (output.screen && output.screen.name === primary) {
                    liriCompositor.defaultOutput = output;
                    return;
                }
            }
        }
    }

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
        keyboardRepeatRate: keyboardSettings.repeatRate
        keyboardRepeatDelay: keyboardSettings.repeatDelay
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
}
