/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
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
import QtQuick.Window 2.2
import QtWayland.Compositor 1.0
import Liri.Launcher 0.1 as Launcher
import Liri.WaylandServer 1.0 as LiriWayland
import Liri.WaylandServer.Private 1.0 as LWSP
import Liri.Shell 1.0
import Vibe.PulseAudio 1.0
import Vibe.PolicyKit 1.0
import "base"
import "windows"

WaylandCompositor {
    id: liriCompositor

    property point mousePos: Qt.point(0, 0)

    property int idleInhibit: 0

    readonly property alias screenManager: screenManager

    readonly property alias settings: settings
    readonly property alias shellSurfaces: shellSurfaces

    readonly property bool hasMaxmizedShellSurfaces: __private.maximizedShellSurfaces > 0
    readonly property bool hasFullscreenShellSurfaces: __private.fullscreenShellSurfaces > 0

    property Component outputConfigurationComponent: OutputConfiguration {}

    readonly property alias applicationManager: applicationManager
    readonly property alias shellHelper: shellHelper
    readonly property alias policyKitAgent: policyKitAgent

    signal shellSurfaceCreated(ShellSurface shellSurface)
    signal shellSurfaceDestroyed(ShellSurface shellSurface)

    defaultSeat.keymap {
        layout: settings.keyboard.layouts[0] ? settings.keyboard.layouts[0] : "us"
        variant: settings.keyboard.variants[0] ? settings.keyboard.variants[0] : ""
        options: settings.keyboard.options[0] ? settings.keyboard.options[0] : ""
        model: settings.keyboard.model
        rules: settings.keyboard.rules[0] ? settings.keyboard.rules[0] : ""
    }

    onCreatedChanged: {
        if (liriCompositor.created) {
            console.debug("Compositor created");
            if (xwaylandLoader.status == Loader.Ready)
                xwaylandLoader.item.startServer();
            else
                shellHelper.start(liriCompositor.socketName);
        }
    }

    onSurfaceRequested: {
        var surface = surfaceComponent.createObject(liriCompositor, {});
        surface.initialize(liriCompositor, client, id, version);
    }

    QtObject {
        id: __private

        property int maximizedShellSurfaces: 0
        property int fullscreenShellSurfaces: 0

        function createShellSurfaceItem(shellSurface, component, output) {
            var parentSurfaceItem = output.viewsBySurface[shellSurface.parentWlSurface];
            var parent = parentSurfaceItem || output.surfacesArea;
            var item = component.createObject(parent, {
                                                  "compositor": liriCompositor,
                                                  "shellSurface": shellSurface
                                              });
            output.viewsBySurface[shellSurface.surface] = item;
            return item;
        }

        function handleShellSurfaceCreated(shellSurface, component) {
            shellSurfaces.append({"shellSurface": shellSurface});

            for (var i = 0; i < screenManager.count; i++)
                createShellSurfaceItem(shellSurface, component, screenManager.objectAt(i));

            liriCompositor.shellSurfaceCreated(shellSurface);
        }

        function handleShellSurfaceDestroyed(shellSurface) {
            if (shellSurface.maximized)
                maximizedShellSurfaces--;
            if (shellSurface.fullscreen)
                fullscreenShellSurfaces--;

            for (var i = 0; i < shellSurfaces.count; i++) {
                if (shellSurfaces.get(i).shellSurface === shellSurface) {
                    shellSurfaces.remove(i, 1);
                    break;
                }
            }

            applicationManager.unregisterShellSurface(shellSurface);

            liriCompositor.shellSurfaceDestroyed(shellSurface);
        }
    }

    ScreenManager {
        id: screenManager

        delegate: Output {
            compositor: liriCompositor
            screen: screenItem.screen
            windowScreen: screenItem.screen
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

    ListModel {
        id: shellSurfaces
    }

    /*
     * Extensions
     */

    QtWindowManager {
        showIsFullScreen: false
        onOpenUrl: processRunner.launchCommand("xdg-open %1".arg(url))
    }

    ShellHelper {
        id: shellHelper

        property WaylandSurface grabSurface: null

        onGrabSurfaceAdded: grabSurface = surface
    }

    LWSP.WlShell {
        id: wlShell

        onWlShellSurfaceRequested: {
            var shellSurface = wlShellSurfaceComponent.createObject(wlShell);
            shellSurface.initialize(wlShell, surface, resource);
        }
        onWlShellSurfaceCreated: __private.handleShellSurfaceCreated(shellSurface, chromeComponent)
    }

    LWSP.XdgShellV5 {
        id: xdgShellV5

        onXdgSurfaceRequested: {
            var xdgSurface = xdgSurfaveV5Component.createObject(xdgShellV5);
            xdgSurface.initialize(xdgShellV5, surface, resource);
        }
        onXdgSurfaceCreated: __private.handleShellSurfaceCreated(xdgSurface, chromeComponent)
        onXdgPopupRequested: {
            var xdgPopup = xdgPopupV5Component.createObject(xdgShellV5);
            xdgPopup.initialize(xdgShellV5, surface, parent, position, resource);
        }
        onXdgPopupCreated: __private.handleShellSurfaceCreated(xdgPopup, chromeComponent)
    }

    LiriWayland.GtkShell {
        id: gtkShell

        onGtkSurfaceRequested: {
            var gtkSurface = gtkSurfaceComponent.createObject(gtkShell);
            gtkSurface.initialize(gtkShell, surface, resource);
        }
    }

    TextInputManager {}

    LiriWayland.OutputManagement {
        id: outputManagement
        onCreateOutputConfiguration: {
            var outputConfiguration = outputConfigurationComponent.createObject();
            outputConfiguration.initialize(outputManagement, resource);
        }
    }

    LiriWayland.Screencaster {
        id: screencaster
    }

    LiriWayland.Screenshooter {
        id: screenshooter

        onCaptureRequested: {
            // TODO: We might want to do something depending on the capture type - plfiorini
            switch (screenshot.captureType) {
            case Screenshot.CaptureActiveWindow:
            case Screenshot.CaptureWindow:
            case Screenshot.CaptureArea:
                break;
            default:
                break;
            }

            // Setup client buffer
            screenshot.setup();
        }
    }

    /*
     * Components
     */

    // Surface component
    Component {
        id: surfaceComponent

        WaylandSurface {
            id: surface

            Component.onDestruction: {
                for (var i = 0; i < screenManager.count; i++)
                    delete screenManager.objectAt(i).viewsBySurface[surface];
            }
        }
    }

    // Custom wl_shell surface
    Component {
        id: wlShellSurfaceComponent

        WlShellWindow {}
    }

    // Custom xdg_shell v5 surface
    Component {
        id: xdgSurfaveV5Component

        XdgSurfaceV5Window {}
    }

    // Custom xdg_shell v5 popup
    Component {
        id: xdgPopupV5Component

        XdgPopupV5Window {}
    }

    // Custom gtk_shell surface
    Component {
        id: gtkSurfaceComponent

        LiriWayland.GtkSurface {
            id: gtkSurface

            onAppIdChanged: {
                for (var i = 0; i < shellSurfaces.count; i++) {
                    var shellSurface = shellSurfaces.get(i).shellSurface;

                    if (shellSurface.surface === gtkSurface.surface) {
                        // Move surface under this appId because for some reason Gtk+ applications
                        // are unable to provide a reliable appId via xdg-shell as opposed to gtk-shell
                        shellSurface.canonicalAppId = appId;

                        // Remove drop shadow and decoration for Gtk+ programs
                        shellSurface.decorated = false;
                        shellSurface.hasDropShadow = false;

                        break;
                    }
                }
            }
        }
    }

    // Window component
    Component {
        id: chromeComponent

        WaylandChrome {
            inputEventsEnabled: !output.screenView.locked
        }
    }

    /*
     * XWayland
     */

    Loader {
        id: xwaylandLoader
        active: true
        sourceComponent: XWayland {}
    }

    /*
     * Miscellaneous
     */

    // Holds move items in the compositor space
    Item {
        id: rootItem
    }

    Launcher.ApplicationManager {
        id: applicationManager
    }

    Launcher.LauncherModel {
        id: launcherModel
        sourceModel: applicationManager
    }

    DateTime {
        id: dateTime
    }

    ShellSettings {
        id: settings
    }

    Launcher.ProcessRunner {
        id: processRunner
    }

    KeyBindings {}

    VolumeControl {
        id: volumeControl
    }

    // PolicyKit
    PolicyKitAgent {
        id: policyKitAgent
        onAuthenticationInitiated: {
            var authDialog = liriCompositor.defaultOutput.screenView.authDialog;
            authDialog.actionId = actionId;
            authDialog.message = message;
            authDialog.iconName = iconName;
            authDialog.realName = realName;
        }
        onAuthenticationRequested: {
            var authDialog = liriCompositor.defaultOutput.screenView.authDialog;
            authDialog.prompt = prompt;
            authDialog.echo = echo;
            authDialog.open();
        }
        onAuthenticationCanceled: liriCompositor.defaultOutput.screenView.authDialog.close()
        onAuthenticationFinished: liriCompositor.defaultOutput.screenView.authDialog.close()
        onAuthorizationGained: liriCompositor.defaultOutput.screenView.authDialog.close()
        onAuthorizationFailed: {
            var authDialog = liriCompositor.defaultOutput.screenView.authDialog;
            authDialog.errorMessage = qsTr("Sorry, that didn't work. Please try again.");
        }
        onAuthorizationCanceled: liriCompositor.defaultOutput.screenView.authDialog.close()
        onInformation: liriCompositor.defaultOutput.screenView.authDialog.infoMessage = message
        onError: liriCompositor.defaultOutput.screenView.authDialog.errorMessage = message

        Component.onCompleted: registerAgent()
    }

    Timer {
        id: idleTimer
        interval: settings.session.idleDelay * 1000
        running: true
        repeat: true
        onTriggered: {
            var i, output, idleHint = false;
            for (i = 0; i < screenManager.count; i++) {
                output = screenManager.objectAt(i);
                if (idleInhibit + output.idleInhibit == 0) {
                    output.idle();
                    idleHint = true;
                }
            }

            SessionInterface.idle = idleHint;
        }
    }

    /*
     * Methods
     */

    function wake() {
        var i;
        for (i = 0; i < screenManager.count; i++) {
            idleTimer.restart();
            screenManager.objectAt(i).wake();
        }

        SessionInterface.idle = false;
    }

    function idle() {
        var i;
        for (i = 0; i < screenManager.count; i++)
            screenManager.objectAt(i).idle();

        SessionInterface.idle = true;
    }

    function activateShellSurfaces(appId) {
        for (var i = 0; i < shellSurfaces.count; i++) {
            var shellSurface = shellSurfaces.get(i).shellSurface;
            if (shellSurface.canonicalAppId === appId) {
                shellSurface.minimized = false;
                for (var j = 0; j < screenManager.count; j++)
                    screenManager.objectAt(j).viewsBySurface[shellSurface.surface].takeFocus();
            }
        }
    }
}
