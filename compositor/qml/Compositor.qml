/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtWayland.Compositor 1.0
import Liri.Launcher 0.1 as Launcher
import Liri.WaylandServer 1.0
import Liri.WaylandServer.Private 1.0 as LWSP
import Liri.XWayland 1.0
import Liri.Shell 1.0
import Vibe.PulseAudio 1.0
import Vibe.PolicyKit 1.0
import "base"
import "windows"

WaylandCompositor {
    id: compositor

    property point mousePos: Qt.point(0, 0)

    property int idleInhibit: 0

    readonly property alias settings: settings
    readonly property alias outputs: __private.outputs
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
        if (compositor.created)
            shellHelper.start(compositor.socketName);
    }

    onSurfaceRequested: {
        var surface = surfaceComponent.createObject(compositor, {});
        surface.initialize(compositor, client, id, version);
    }

    QtObject {
        id: __private

        property var outputs: []
        property int maximizedShellSurfaces: 0
        property int fullscreenShellSurfaces: 0

        function createShellSurfaceItem(shellSurface, component, moveItem, output) {
            var parentSurfaceItem = output.viewsBySurface[shellSurface.parentWlSurface];
            var parent = parentSurfaceItem || output.surfacesArea;
            var item = component.createObject(parent, {
                                                  "compositor": compositor,
                                                  "shellSurface": shellSurface,
                                                  "moveItem": moveItem
                                              });
            if (shellSurface.windowType === Qt.Popup) {
                item.x = shellSurface.offset.x;
                item.y = shellSurface.offset.y;
            }
            output.viewsBySurface[shellSurface.surface] = item;
            return item;
        }

        function handleShellSurfaceCreated(shellSurface, component) {
            shellSurfaces.append({"shellSurface": shellSurface});

            var moveItem =
                    moveItemComponent.createObject(rootItem, {
                                                       "x": defaultOutput.position.x,
                                                       "y": defaultOutput.position.y,
                                                       "width": Qt.binding(function() { return shellSurface.surface.width; }),
                                                       "height": Qt.binding(function() { return shellSurface.surface.height; })
                                                   });
            for (var i = 0; i < outputs.length; i++)
                createShellSurfaceItem(shellSurface, component, moveItem, outputs[i]);

            compositor.shellSurfaceCreated(shellSurface);
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

            compositor.shellSurfaceDestroyed(shellSurface);
        }
    }

    ScreenManager {
        id: screenManager

        onScreenAdded: {
            var view = outputComponent.createObject(compositor, {
                                                        "compositor": compositor,
                                                        "nativeScreen": screen
                                                    })

            __private.outputs.push(view);
        }
        onScreenRemoved: {
            var index = screenManager.indexOf(screen);

            if (index < __private.outputs.length) {
                var output = __private.outputs[index];
                __private.outputs.splice(index, 1);
                output.destroy();
            }
        }
        onPrimaryScreenChanged: {
            var index = screenManager.indexOf(screen);

            if (index < __private.outputs.length)
                compositor.defaultOutput = __private.outputs[index];
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

    GtkShell {
        id: gtkShell

        onGtkSurfaceRequested: {
            var gtkSurface = gtkSurfaceComponent.createObject(gtkShell);
            gtkSurface.initialize(gtkShell, surface, resource);
        }
    }

    TextInputManager {}

    OutputManagement {
        id: outputManagement
        onCreateOutputConfiguration: {
            var outputConfiguration = outputConfigurationComponent.createObject();
            outputConfiguration.initialize(outputManagement, resource);
        }
    }

    Screencaster {
        id: screencaster
    }

    Screenshooter {
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

    // Output component
    Component {
        id: outputComponent

        Output {}
    }

    // Surface component
    Component {
        id: surfaceComponent

        WaylandSurface {
            id: surface

            Component.onDestruction: {
                for (var i = 0; i < __private.outputs.length; i++)
                    delete __private.outputs[i].viewsBySurface[surface];
            }
        }
    }

    // Custom wl_shell surface
    Component {
        id: wlShellSurfaceComponent

        LWSP.WlShellSurface {
            id: wlShellSurface

            readonly property alias mapped: details.mapped
            property bool activated: true
            property bool minimized: false
            property bool maximized: false
            property bool fullscreen: false
            property bool hasDropShadow: !maximized && !fullscreen
            property string canonicalAppId: applicationManager.canonicalizeAppId(className)

            property WaylandSurface parentWlSurface: null
            property point offset

            readonly property alias responsive: details.responsive

            onActivatedChanged: {
                if (details.registered && activated)
                    applicationManager.focusShellSurface(wlShellSurface);
            }
            onCanonicalAppIdChanged: {
                if (!details.registered && canonicalAppId) {
                    applicationManager.registerShellSurface(wlShellSurface);
                    details.registered = true;

                    if (activated)
                        applicationManager.focusShellSurface(wlShellSurface);
                }
            }
            onSetDefaultToplevel: {
                maximized = false;
                fullscreen = false;
                parentWlSurface = null;
                offset.x = 0;
                offset.y = 0;
            }
            onSetPopup: {
                maximized = false;
                fullscreen = false;
                parentWlSurface = parentSurface;
                offset.x = relativeToParent.x;
                offset.y = relativeToParent.y;
            }
            onSetTransient: {
                maximized = false;
                fullscreen = false;
                parentWlSurface = parentSurface;
                offset.x = relativeToParent.x;
                offset.y = relativeToParent.y;
            }
            onSetMaximized: {
                maximized = true;
            }
            onSetFullScreen: {
                fullscreen = true;
            }
            onMaximizedChanged: {
                if (maximized)
                    __private.maximizedShellSurfaces++;
                else
                    __private.maximizedShellSurfaces--;
            }
            onFullscreenChanged: {
                if (fullscreen)
                    __private.fullscreenShellSurfaces++;
                else
                    __private.fullscreenShellSurfaces--;
            }
            onPong: {
                pingTimer.stop();
                details.responsive = true;
            }

            QtObject {
                id: details

                property bool mapped: false
                property bool registered: false
                property bool responsive: true
            }

            Timer {
                id: pingTimer
                interval: 500
                onTriggered: details.responsive = false
            }

            Connections {
                target: surface
                onHasContentChanged: {
                    if (surface.hasContent) {
                        // Set mapped flag which will make Chrome play the animation
                        details.mapped = surface.hasContent;
                    }
                }
                onSurfaceDestroyed: {
                    // Unset mapped flag which will make Chrome play the animation
                    details.mapped = false;
                }
            }

            Connections {
                target: defaultSeat
                onKeyboardFocusChanged: wlShellSurface.activated = newFocus == surface
            }

            function pingClient() {
                wlShellSurface.ping();
                pingTimer.start();
            }

            function close() {
                if (windowType == Qt.Popup)
                    sendPopupDone();
                else
                    surface.destroy();
            }

            Component.onDestruction: {
                __private.handleShellSurfaceDestroyed(wlShellSurface);
            }
        }
    }

    // Custom xdg_shell v5 surface
    Component {
        id: xdgSurfaveV5Component

        LWSP.XdgSurfaceV5 {
            id: xdgSurface

            readonly property alias mapped: details.mapped
            property bool minimized: false
            property bool hasDropShadow: !maximized && !fullscreen
            property string canonicalAppId: applicationManager.canonicalizeAppId(appId)

            property WaylandSurface parentWlSurface: parentSurface ? parentSurface.surface : null
            property point offset

            readonly property alias responsive: details.responsive

            onActivatedChanged: {
                if (details.registered && activated)
                    applicationManager.focusShellSurface(xdgSurface);
            }
            onCanonicalAppIdChanged: {
                if (!details.registered && canonicalAppId) {
                    applicationManager.registerShellSurface(xdgSurface);
                    details.registered = true;

                    if (activated)
                        applicationManager.focusShellSurface(xdgSurface);
                }
            }
            onSetTopLevel: {
                offset.x = 0;
                offset.y = 0;
            }
            onSetMinimized: minimized = true
            onSetMaximized: __private.maximizedShellSurfaces++
            onUnsetMaximized: __private.maximizedShellSurfaces--
            onSetFullscreen: __private.fullscreenShellSurfaces++
            onUnsetFullscreen: __private.fullscreenShellSurfaces--

            QtObject {
                id: details

                property bool mapped: false
                property bool registered: false
                property bool responsive: true
            }

            Timer {
                id: pingTimer
                interval: 500
                onTriggered: details.responsive = false
            }

            Connections {
                target: surface
                onHasContentChanged: {
                    if (surface.hasContent) {
                        // Center transient windows
                        if (xdgSurface.windowType === Qt.SubWindow) {
                            offset.x = (xdgSurface.parentWlSurface.size.width - xdgSurface.surface.size.width) / 2;
                            offset.y = (xdgSurface.parentWlSurface.size.height - xdgSurface.surface.size.height) / 2;
                        }

                        // Set mapped flag which will make Chrome play the animation
                        details.mapped = surface.hasContent;
                    }
                }
            }

            Connections {
                target: shell
                onPong: {
                    pingTimer.stop();
                    details.responsive = true;
                }
            }

            function pingClient() {
                shell.ping(surface.client);
                pingTimer.start();
            }

            function close() {
                sendClose();
            }

            Component.onDestruction: {
                __private.handleShellSurfaceDestroyed(xdgSurface);

                // Unset mapped flag which will make Chrome play the animation
                details.mapped = false;
            }
        }
    }

    // Custom xdg_shell v5 popup
    Component {
        id: xdgPopupV5Component

        LWSP.XdgPopupV5 {
            id: xdgPopup

            readonly property alias mapped: details.mapped
            property bool hasDropShadow: false
            property string canonicalAppId

            property WaylandSurface parentWlSurface: parentSurface
            property point offset: position

            QtObject {
                id: details

                property bool mapped: false
            }

            Connections {
                target: surface
                onHasContentChanged: {
                    if (surface.hasContent) {
                        // Set mapped flag which will make Chrome play the animation
                        details.mapped = surface.hasContent;
                    }
                }
            }

            Component.onDestruction: {
                __private.handleShellSurfaceDestroyed(xdgPopup);

                // Unset mapped flag which will make Chrome play the animation
                details.mapped = false;
            }
        }
    }

    // Custom gtk_shell surface
    Component {
        id: gtkSurfaceComponent

        GtkSurface {
            id: gtkSurface

            onAppIdChanged: {
                for (var i = 0; i < shellSurfaces.count; i++) {
                    var shellSurface = shellSurfaces.get(i).shellSurface;

                    if (shellSurface.surface === gtkSurface.surface) {
                        // Move surface under this appId because for some reason Gtk+ applications
                        // are unable to provide a reliable appId via xdg-shell as opposed to gtk-shell
                        shellSurface.canonicalAppId = appId;

                        // Remove drop shadow for Gtk+ programs
                        shellSurface.hasDropShadow = false;

                        break;
                    }
                }
            }
        }
    }

    // Item that catches move operations instead of actual surface items
    Component {
        id: moveItemComponent

        MoveItem {}
    }

    // Window component
    Component {
        id: chromeComponent

        Chrome {
            inputEventsEnabled: !output.screenView.locked
        }
    }

    /*
     * XWayland
     */

    XWayland {
        id: xwayland
        enabled: true
        onShellSurfaceCreated: __private.handleShellSurfaceCreated(shellSurface, xchromeComponent)
    }

    Component {
        id: xchromeComponent

        XWaylandChrome {
            inputEventsEnabled: !output.screenView.locked
        }
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
        applicationManager: applicationManager
    }

    Launcher.AppsModel {
        id: appsModel
        applicationManager: applicationManager
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
            var authDialog = compositor.defaultOutput.screenView.authDialog;
            authDialog.actionId = actionId;
            authDialog.message = message;
            authDialog.iconName = iconName;
            authDialog.realName = realName;
        }
        onAuthenticationRequested: {
            var authDialog = compositor.defaultOutput.screenView.authDialog;
            authDialog.prompt = prompt;
            authDialog.echo = echo;
            authDialog.open();
        }
        onAuthenticationCanceled: compositor.defaultOutput.screenView.authDialog.close()
        onAuthenticationFinished: compositor.defaultOutput.screenView.authDialog.close()
        onAuthorizationGained: compositor.defaultOutput.screenView.authDialog.close()
        onAuthorizationFailed: {
            var authDialog = compositor.defaultOutput.screenView.authDialog;
            authDialog.errorMessage = qsTr("Sorry, that didn't work. Please try again.");
        }
        onAuthorizationCanceled: compositor.defaultOutput.screenView.authDialog.close()
        onInformation: compositor.defaultOutput.screenView.authDialog.infoMessage = message
        onError: compositor.defaultOutput.screenView.authDialog.errorMessage = message

        Component.onCompleted: registerAgent()
    }

    Timer {
        id: idleTimer
        interval: settings.session.idleDelay * 1000
        running: true
        repeat: true
        onTriggered: {
            var i, output, idleHint = false;
            for (i = 0; i < __private.outputs.length; i++) {
                output = __private.outputs[i];
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
        for (i = 0; i < __private.outputs.length; i++) {
            idleTimer.restart();
            __private.outputs[i].wake();
        }

        SessionInterface.idle = false;
    }

    function idle() {
        var i;
        for (i = 0; i < __private.outputs.length; i++)
            __private.outputs[i].idle();

        SessionInterface.idle = true;
    }

    function activateShellSurfaces(appId) {
        for (var i = 0; i < shellSurfaces.count; i++) {
            var shellSurface = shellSurfaces.get(i).shellSurface;
            if (shellSurface.canonicalAppId === appId) {
                shellSurface.minimized = false;
                for (var j = 0; j < __private.outputs.length; j++)
                    __private.outputs[j].viewsBySurface[shellSurface.surface].takeFocus();
            }
        }
    }
}
