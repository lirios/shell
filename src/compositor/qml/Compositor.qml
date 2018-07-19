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
import Liri.Launcher 1.0 as Launcher
import Liri.Shell 1.0
import Liri.PolicyKit 1.0
import Liri.private.shell 1.0 as P
import "base"
import "windows"

WaylandCompositor {
    id: liriCompositor

    property point mousePos: Qt.point(0, 0)

    property int idleInhibit: 0

    readonly property alias screenManager: screenManager
    readonly property alias outputManager: outputManager

    readonly property alias settings: settings
    readonly property alias shellSurfaces: shellSurfaces

    property var activeShellSurface: null
    readonly property bool hasMaxmizedShellSurfaces: __private.maximizedShellSurfaces > 0
    readonly property bool hasFullscreenShellSurfaces: __private.fullscreenShellSurfaces > 0

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
            hardwareScreen: screenItem
            uuid: screenItem.uuid
            position: Qt.point(x, y)
            manufacturer: screenItem.manufacturer
            model: screenItem.model
            physicalSize: screenItem.physicalSize
            subpixel: screenItem.subpixel
            transform: screenItem.transform
            scaleFactor: screenItem.scaleFactor
            modes: screenItem.modes
            currentModeIndex: screenItem.currentModeIndex
            preferredModeIndex: screenItem.preferredModeIndex

            Component.onCompleted: {
                // Set default output the first time
                if (!liriCompositor.defaultOutput && screenItem.primary)
                    liriCompositor.defaultOutput = this;
            }
        }

        function getOutputForUuid(uuid) {
            for (var i = 0; i < screenManager.count; i++) {
                var output = screenManager.objectAt(i);
                if (output.uuid === uuid)
                    return output;
            }

            return null;
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

    P.WlShell {
        id: wlShell

        onWlShellSurfaceRequested: {
            var shellSurface = wlShellSurfaceComponent.createObject(wlShell);
            shellSurface.initialize(wlShell, surface, resource);
        }
        onWlShellSurfaceCreated: __private.handleShellSurfaceCreated(shellSurface, chromeComponent)
    }

    P.XdgShellV5 {
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

    P.GtkShell {
        id: gtkShell

        onGtkSurfaceRequested: {
            var gtkSurface = gtkSurfaceComponent.createObject(gtkShell);
            gtkSurface.initialize(gtkShell, surface, resource);
        }
    }

    TextInputManager {}

    P.OutputManagement {
        id: outputManager
        onPrimaryOutputDeviceChanged: {
            var output = screenManager.getOutputForUuid(device.uuid);
            if (output)
                liriCompositor.defaultOutput = output;
        }
        onOutputConfigurationCreated: {
            configuration.changeRequested.connect(function() {
                var failedCount = this.changes.length;

                for (var i = 0; i < this.changes.length; i++) {
                    var changeset = this.changes[i];

                    if (changeset.empty) {
                        // No changes, we are done!
                        failedCount--;
                        continue;
                    }

                    var output = screenManager.getOutputForUuid(changeset.outputDevice.uuid);
                    if (!output) {
                        // No output found with that uuid
                        continue;
                    }

                    if (changeset.enabledChanged)
                        output.enabled = changeset.enabled;

                    if (changeset.transformChanged)
                        output.hardwareScreen.transform = changeset.transform;

                    if (changeset.currentModeIndexChanged)
                        output.hardwareScreen.currentModeIndex = changeset.currentModeIndex;

                    if (changeset.positionChanged)
                        output.hardwareScreen.position = changeset.position;

                    if (changeset.scaleFactorChanged)
                        output.hardwareScreen.scaleFactor = changeset.scaleFactor;

                    failedCount--;
                }

                if (failedCount === 0)
                    this.setApplied();
                else
                    this.setFailed();
            });
        }
    }

    P.ScreenCast {}

    P.Screenshooter {
        id: screenshooter

        onCaptureRequested: {
            // Grab cursor surface
            screenshot.grabCursorItem(defaultOutput.cursor);

            // Capture
            switch (screenshot.captureType) {
            case P.Screenshot.CaptureScreens:
                screenshot.grabScreens();
                break;
            case P.Screenshot.CaptureActiveWindow:
                screenshot.grabSurfaceItem(defaultOutput.viewsBySurface[activeShellSurface.surface]);
                break;
            case P.Screenshot.CaptureWindow:
            case P.Screenshot.CaptureArea:
                // TODO: Implement interactive capture types
                break;
            default:
                break;
            }

            // Save capture
            screenshot.save();
            delete screenshot;
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

        P.GtkSurface {
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
        onShellSurfaceFocused: activeShellSurface = shellSurface
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
