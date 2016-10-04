/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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

import QtQuick 2.5
import GreenIsland 1.0 as GreenIsland
import Vibe.PolicyKit 1.0

BaseCompositor {
    id: compositor

    property Component screenViewComponent

    outputComponent: DesktopOutput {
        screenViewComponent: compositor.screenViewComponent
    }

    property Component outputConfigurationComponent: OutputConfiguration {}

    readonly property alias windowsModel: windowsModel
    readonly property alias applicationManager: applicationManager
    readonly property alias policyKitAgent: policyKitAgent

    signal openUrl(var url)

    onSurfaceRequested: {
        var surface = surfaceComponent.createObject(compositor, {});
        surface.initialize(compositor, client, id, version);
    }

    /*
     * Extensions
     */

    GreenIsland.QtWindowManager {
        showIsFullScreen: false
        onOpenUrl: compositor.openUrl(url)
    }

    GreenIsland.WlShell {
        onWlShellSurfaceCreated: {
            var window = applicationManager.createWindow(shellSurface.surface)

            for (var i = 0; i < __private.outputs.length; i++) {
                var view = chromeComponent.createObject(__private.outputs[i].surfacesArea, {
                    "shellSurface": shellSurface, "window": window, "decorated": true
                })

                view.moveItem = window.moveItem
                window.addWindowView(view)
            }
        }
    }

    GreenIsland.XdgShell {
        property variant viewsBySurface: ({})

        onXdgSurfaceCreated: {
            var window = applicationManager.createWindow(xdgSurface.surface)

            var i, view;
            for (i = 0; i < __private.outputs.length; i++) {
                view = chromeComponent.createObject(__private.outputs[i].surfacesArea, {
                    "shellSurface": xdgSurface, "window": window, "decorated": false
                })

                view.moveItem = window.moveItem

                if (viewsBySurface[xdgSurface.surface] == undefined)
                    viewsBySurface[xdgSurface.surface] = new Array()

                viewsBySurface[xdgSurface.surface].push({
                    "output": __private.outputs[i], "view": view
                })
                window.addWindowView(view)
            }
        }
        onXdgPopupCreated: {
            var window = applicationManager.createWindow(xdgPopup.surface);

            var i, j, parentView, view, parentViews = viewsBySurface[xdgPopup.parentSurface];
            for (i = 0; i < __private.outputs.length; i++) {
                for (j = 0; j < parentViews.length; j++) {
                    if (parentViews[j].output == __private.outputs[i]) {
                        view = chromeComponent.createObject(parentViews[j].view, {"shellSurface": xdgPopup, "window": window});
                        view.x = xdgPopup.position.x;
                        view.y = xdgPopup.position.y;
                        view.moveItem = window.moveItem;
                        window.addWindowView(view);
                    }
                }
            }
        }
    }

    GreenIsland.GtkShell {}

    GreenIsland.TextInputManager {}

    GreenIsland.ApplicationManager {
        id: applicationManager
    }

    GreenIsland.OutputManagement {
        id: outputManagement
        onCreateOutputConfiguration: {
            var outputConfiguration = outputConfigurationComponent.createObject()
            outputConfiguration.initialize(outputManagement, resource)
        }
    }

    GreenIsland.Screencaster {
        id: screencaster
    }

    GreenIsland.Screenshooter {
        id: screenshooter

        onCaptureRequested: {
            // TODO: We might want to do something depending on the capture type - plfiorini
            switch (screenshot.captureType) {
            case GreenIsland.Screenshot.CaptureActiveWindow:
            case GreenIsland.Screenshot.CaptureWindow:
            case GreenIsland.Screenshot.CaptureArea:
                break
            default:
                break
            }

            // Setup client buffer
            screenshot.setup()
        }
    }

    // Windows model
    ListModel {
        id: windowsModel
    }

    // XWayland
    Loader {
        active: false
        asynchronous: true

        sourceComponent: Qt.createComponent(Qt.resolvedUrl("XWayland.qml"), Component.Asynchronous)

        onStatusChanged: {
            if (status === Loader.Error)
                console.warn("Error loading XWayland support:", sourceComponent.errorString())
        }
    }

    // PolicyKit
    PolicyKitAgent {
        id: policyKitAgent
        onAuthenticationInitiated: {
            var authDialog = compositor.defaultOutput.screenView.authDialog
            authDialog.actionId = actionId
            authDialog.message = message
            authDialog.iconName = iconName
            authDialog.realName = realName
        }
        onAuthenticationRequested: {
            var authDialog = compositor.defaultOutput.screenView.authDialog
            authDialog.prompt = prompt
            authDialog.echo = echo
            authDialog.open()
        }
        onAuthenticationCanceled: compositor.defaultOutput.screenView.authDialog.close()
        onAuthenticationFinished: compositor.defaultOutput.screenView.authDialog.close()
        onAuthorizationGained: compositor.defaultOutput.screenView.authDialog.close()
        onAuthorizationFailed: {
            var authDialog = compositor.defaultOutput.screenView.authDialog
            authDialog.errorMessage = qsTr("Sorry, that didn't work. Please try again.")
        }
        onAuthorizationCanceled: compositor.defaultOutput.screenView.authDialog.close()
        onInformation: compositor.defaultOutput.screenView.authDialog.infoMessage = message
        onError: compositor.defaultOutput.screenView.authDialog.errorMessage = message

        Component.onCompleted: registerAgent()
    }

    // Surface component
    Component {
        id: surfaceComponent

        GreenIsland.WaylandSurface {
            id: surface

            onMappedChanged: {
                if (cursorSurface)
                    return

                if (isMapped) {
                    var window = applicationManager.windowForSurface(surface)

                    if (window)
                        windowsModel.append({"window": window})
                } else {
                    for (var i = 0; i < windowsModel.count; i++) {
                        if (windowsModel.get(i).window.surface === surface) {
                            windowsModel.remove(i, 1)
                            break
                        }
                    }
                }
            }
        }
    }

    // Window component
    Component {
        id: chromeComponent

        GreenIsland.WindowChrome {
            animationsEnabled: parent.animateWindows
            inputEventsEnabled: !output.screenView.locked
        }
    }
}
