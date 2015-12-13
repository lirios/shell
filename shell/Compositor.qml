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

import QtQuick 2.0
import GreenIsland 1.0 as GreenIsland
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaiios.misc 0.1
import "desktop"

GreenIsland.WaylandCompositor {
    readonly property alias outputs: d.outputs
    readonly property alias primaryScreen: screenManager.primaryScreen

    readonly property alias windowsModel: windowsModel
    readonly property alias applicationManager: applicationManager
    readonly property alias settings: settings

    id: hawaiiCompositor
    extensions: [
        GreenIsland.ApplicationManager {
            id: applicationManager

            Component.onCompleted: {
                initialize()
            }
        }
    ]
    onCreateSurface: {
        var surface = surfaceComponent.createObject(hawaiiCompositor, {});
        surface.initialize(hawaiiCompositor, client, id, version);
    }

    /*
     * Components
     */

    // Private properties
    QtObject {
        id: d

        property variant outputs: []
    }

    // Pointer tracking with global coordinates
    GreenIsland.GlobalPointerTracker {
        id: globalPointerTracker
        compositor: hawaiiCompositor
    }

    // Settings
    ShellSettings {
        id: settings
    }

    // Key bindings
    KeyBindings {}

    // Screen manager
    GreenIsland.ScreenManager {
        id: screenManager
        onScreenAdded: {
            var view = outputComponent.createObject(
                        hawaiiCompositor, {
                            "compositor": hawaiiCompositor,
                            "nativeScreen": screen
                        });
            d.outputs.push(view);
            windowManager.recalculateVirtualGeometry
        }
        onScreenRemoved: {
            var index = screenManager.indexOf(screen);
            if (index < d.outputs.length) {
                var output = d.outputs[index];
                d.outputs.splice(index, 1);
                output.destroy();
                windowManager.recalculateVirtualGeometry();
            }
        }
        onPrimaryScreenChanged: {
            var index = screenManager.indexOf(screen);
            if (index < d.outputs.length)
                hawaiiCompositor.defaultOutput = d.outputs[index];
        }
    }

    // Windows
    ListModel {
        id: windowsModel
    }

    // Window manager
    GreenIsland.WindowManager {
        id: windowManager
        compositor: hawaiiCompositor
        onWindowCreated: {
            var i, output, view;
            for (i = 0; i < d.outputs.length; i++) {
                output = d.outputs[i];
                view = windowComponent.createObject(output.surfacesArea, {"window": window});
                view.initialize(window, output);
            }

            window.typeChanged.connect(function() {
                if (window.type == GreenIsland.ClientWindow.TopLevel)
                    windowsModel.append({"window": window});
            });
        }

        Component.onCompleted: {
            initialize();
        }
    }

    // Surface component
    Component {
        id: surfaceComponent

        GreenIsland.WaylandSurface {}
    }

    // Window component
    Component {
        id: windowComponent

        GreenIsland.WaylandWindow {}
    }

    // Output component
    Component {
        id: outputComponent

        Output {}
    }
}
