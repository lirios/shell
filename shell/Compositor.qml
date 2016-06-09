/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import org.hawaiios.misc 0.1
import "desktop"

GreenIsland.WaylandCompositor {
    readonly property alias outputs: d.outputs
    readonly property alias primaryScreen: screenManager.primaryScreen

    property int idleInhibit: 0

    readonly property alias windowsModel: windowsModel
    readonly property alias applicationManager: applicationManager
    readonly property alias settings: settings

    id: hawaiiCompositor
    extensions: [
        GreenIsland.QtWindowManager {
            showIsFullScreen: false
        },
        GreenIsland.WlShell {
            onShellSurfaceCreated: {
                var window = windowManager.createWindow(shellSurface.surface);
                windowsModel.append({"window": window});

                var i, view;
                for (i = 0; i < d.outputs.length; i++) {
                    view = chromeComponent.createObject(d.outputs[i].surfacesArea, {"shellSurface": shellSurface, "window": window});
                    view.moveItem = window.moveItem;
                    window.addWindowView(view);
                }
            }
        },
        GreenIsland.XdgShell {
            onXdgSurfaceCreated: {
                var window = windowManager.createWindow(xdgSurface.surface);
                windowsModel.append({"window": window});

                var i, view;
                for (i = 0; i < d.outputs.length; i++) {
                    view = chromeComponent.createObject(d.outputs[i].surfacesArea, {"shellSurface": xdgSurface, "window": window});
                    view.moveItem = window.moveItem;
                    window.addWindowView(view);
                }
            }
        },
        GreenIsland.TextInputManager {},
        GreenIsland.ApplicationManager {
            id: applicationManager
        },
        GreenIsland.OutputManagement {
            id: outputManagement
            onCreateOutputConfiguration: {
                var outputConfiguration = outputConfigurationComponent.createObject();
                outputConfiguration.initialize(outputManagement, resource);
            }
        },
        GreenIsland.Screencaster {
            id: screencaster
        },
        GreenIsland.Screenshooter {
            id: screenshooter
            onCaptureRequested: {
                // TODO: We might want to do something depending on the capture type - plfiorini
                switch (screenshot.captureType) {
                case GreenIsland.Screenshot.CaptureActiveWindow:
                case GreenIsland.Screenshot.CaptureWindow:
                case GreenIsland.Screenshot.CaptureArea:
                    break;
                default:
                    break;
                }

                // Setup client buffer
                screenshot.setup();
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
        }
        onScreenRemoved: {
            var index = screenManager.indexOf(screen);
            if (index < d.outputs.length) {
                var output = d.outputs[index];
                d.outputs.splice(index, 1);
                output.destroy();
            }
        }
        onPrimaryScreenChanged: {
            var index = screenManager.indexOf(screen);
            if (index < d.outputs.length) {
                console.debug("Setting screen", index, "as primary");
                hawaiiCompositor.defaultOutput = d.outputs[index];
            }
        }
    }

    // Idle manager
    Timer {
        id: idleTimer
        interval: settings.session.idleDelay * 1000
        running: true
        repeat: true
        onTriggered: {
            var i, output, idleHint = false;
            for (i = 0; i < d.outputs.length; i++) {
                output = d.outputs[i];
                if (idleInhibit + output.idleInhibit == 0) {
                    output.idle();
                    idleHint = true;
                }
            }

            SessionInterface.idle = idleHint;
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
    }

    // Surface component
    Component {
        id: surfaceComponent

        GreenIsland.WaylandSurface {}
    }

    // Window component
    Component {
        id: chromeComponent

        GreenIsland.WindowChrome {}
    }

    // Output component
    Component {
        id: outputComponent

        Output {}
    }

    // Output configuration component
    Component {
        id: outputConfigurationComponent

        OutputConfiguration {}
    }

    /*
     * Methods
     */

    function wake() {
        var i;
        for (i = 0; i < d.outputs.length; i++) {
            idleTimer.restart();
            d.outputs[i].wake();
        }

        SessionInterface.idle = false;
    }

    function idle() {
        var i;
        for (i = 0; i < d.outputs.length; i++)
            d.outputs[i].idle();

        SessionInterface.idle = true;
    }
}
