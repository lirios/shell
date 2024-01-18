// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml
import QtQuick
import QtQuick.Window
import Aurora.Compositor
import Aurora.Platform
import Liri.Shell.Compositor as LS
import Liri.Session as Session
import ".." as Root

WaylandCompositor {
    id: liriCompositor

    onCreatedChanged: {
        if (liriCompositor.created) {
            console.debug("Compositor created");

            Session.SessionManager.setEnvironment("WAYLAND_DISPLAY", liriCompositor.socketName);
            SessionInterface.registerService();
        }
    }

    onSurfaceRequested: {
        var surface = surfaceComponent.createObject(liriCompositor, {});
        surface.initialize(liriCompositor, client, id, version);
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Alt+Backspace"
        onActivated: liriCompositor.quit()
    }

    PlatformOutputsModel {
        id: platformOutputsModel
    }

    Instantiator {
        model: platformOutputsModel
        delegate: ErrorOutput {
            compositor: liriCompositor
            platformOutput: output

            Component.onCompleted: {
                // Set default output the first time
                if (!liriCompositor.defaultOutput)
                    liriCompositor.defaultOutput = this;
            }
        }
    }

    // Surface component
    Component {
        id: surfaceComponent

        WaylandSurface {}
    }

    // Shell helper
    LS.LiriShellV1 {
        id: shellHelper
    }

    function quit() {
        shellHelper.sendQuit();

        for (var i = 0; i < outputs.length; i++)
            outputs[i].window.close();

        Qt.quit();
    }
}
