// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml 2.2
import QtQuick 2.15
import QtQuick.Window 2.15
import QtWayland.Compositor 1.15
import Liri.WaylandServer 1.0 as WS
import Liri.private.shell 1.0 as P
import ".." as Root

P.WaylandCompositor {
    id: liriCompositor

    onCreatedChanged: {
        if (liriCompositor.created) {
            console.debug("Compositor created");

            SessionInterface.setEnvironment("WAYLAND_DISPLAY", liriCompositor.socketName);
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

    P.ScreenModel {
        id: screenModel
        fileName: screenConfigurationFileName
    }

    Instantiator {
        model: screenModel
        delegate: ErrorOutput {
            compositor: liriCompositor
            screen: screenItem
            position: screenItem.position
            manufacturer: screenItem.manufacturer
            model: screenItem.model
            physicalSize: screenItem.physicalSize
            subpixel: screenItem.subpixel
            transform: screenItem.transform
            scaleFactor: screenItem.scaleFactor

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
    WS.LiriShell {
        id: shellHelper
    }

    function quit() {
        shellHelper.sendQuit();

        for (var i = 0; i < outputs.length; i++)
            outputs[i].window.close();

        Qt.quit();
    }
}
