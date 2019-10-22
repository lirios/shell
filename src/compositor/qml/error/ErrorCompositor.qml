/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQml 2.2
import QtQuick 2.5
import QtQuick.Window 2.0
import QtWayland.Compositor 1.0
import Liri.WaylandServer 1.0 as WS
import Liri.private.shell 1.0 as P
import ".." as Root

WaylandCompositor {
    id: liriCompositor

    onCreatedChanged: {
        if (liriCompositor.created) {
            console.debug("Compositor created");

            SessionInterface.setEnvironment("WAYLAND_DISPLAY", liriCompositor.socketName);
            SessionInterface.setEnvironment("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1");
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

    Root.ScreenManager {
        id: screenManager

        delegate: ErrorOutput {
            compositor: liriCompositor
            screen: screenItem.screen
            primary: screenItem.primary
            position: Qt.point(x, y)
            manufacturer: screenItem.manufacturer
            model: screenItem.model
            physicalSize: screenItem.physicalSize
            subpixel: screenItem.subpixel
            transform: screenItem.transform
            scaleFactor: screenItem.scaleFactor
            currentModeIndex: screenItem.currentModeIndex
            preferredModeIndex: screenItem.preferredModeIndex

            Component.onCompleted: {
                // Add modes
                var sourceModes = screenManager.screenModel.get(index).modes;
                for (var i = 0; i < sourceModes.length; i++)
                    modes.push(sourceModes[i]);

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
        onGrabSurfaceAdded: {
            for (var i = 0; i < screenManager.count; i++)
                screenManager.objectAt(i).grabItem.surface = surface;
        }
    }

    function quit() {
        shellHelper.sendQuit();

        for (var i = 0; i < screenManager.count; i++)
            screenManager.objectAt(i).window.close();

        Qt.quit();
    }
}
