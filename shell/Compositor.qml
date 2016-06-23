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
import org.hawaiios.launcher 0.1 as CppLauncher
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
            onOpenUrl: {
                // Execute url with xdg-open
                console.warn("Run", url, "with xdg-open");
                processRunner.launchCommand("xdg-open " + url);
            }
        },
        GreenIsland.WlShell {
            onShellSurfaceCreated: {
                var window = applicationManager.createWindow(shellSurface.surface);

                var i, view;
                for (i = 0; i < d.outputs.length; i++) {
                    view = chromeComponent.createObject(d.outputs[i].surfacesArea, {"shellSurface": shellSurface, "window": window, "decorated": true});
                    view.moveItem = window.moveItem;
                    window.addWindowView(view);
                }
            }
        },
        GreenIsland.XdgShell {
            property variant viewsBySurface: ({})

            onXdgSurfaceCreated: {
                var window = applicationManager.createWindow(xdgSurface.surface);

                var i, view;
                for (i = 0; i < d.outputs.length; i++) {
                    view = chromeComponent.createObject(d.outputs[i].surfacesArea, {"shellSurface": xdgSurface, "window": window, "decorated": false});
                    view.moveItem = window.moveItem;
                    if (viewsBySurface[xdgSurface.surface] == undefined)
                        viewsBySurface[xdgSurface.surface] = new Array();
                    viewsBySurface[xdgSurface.surface].push({"output": d.outputs[i], "view": view});
                    window.addWindowView(view);
                }
            }
            onXdgPopupCreated: {
                var window = applicationManager.createWindow(xdgPopup.surface);

                var i, j, parentView, view, parentViews = viewsBySurface[xdgPopup.parentSurface];
                for (i = 0; i < d.outputs.length; i++) {
                    for (j = 0; j < parentViews.length; j++) {
                        if (parentViews[j].output == d.outputs[i]) {
                            view = chromeComponent.createObject(parentViews[j].view, {"shellSurface": xdgPopup, "window": window});
                            view.x = xdgPopup.position.x;
                            view.y = xdgPopup.position.y;
                            view.moveItem = window.moveItem;
                            window.addWindowView(view);
                        }
                    }
                }
            }
        },
        GreenIsland.GtkShell {},
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

    // Process launcher
    CppLauncher.ProcessRunner {
        id: processRunner
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

    // Surface component
    Component {
        id: surfaceComponent

        GreenIsland.WaylandSurface {
            id: surface
            onMappedChanged: {
                if (!cursorSurface) {
                    if (isMapped) {
                        var window = applicationManager.windowForSurface(surface);
                        if (window)
                            windowsModel.append({"window": window});
                    } else {
                        for (var i = 0; i < windowsModel.count; i++) {
                            if (windowsModel.get(i).window.surface === surface) {
                                windowsModel.remove(i, 1);
                                break;
                            }
                        }
                    }
                }
            }
        }
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
