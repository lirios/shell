/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2012-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtWayland.Compositor 1.0
import Liri.Launcher 0.1 as Launcher
import Liri.WaylandServer 1.0
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

    readonly property bool hasMaxmizedShellSurfaces: {
        for (var i = 0; i < shellSurfaces.count; i++) {
            var shellSurface = shellSurfaces[i].shellSurface;
            if (shellSurface.maximized)
                return true;
        }

        return false;
    }

    readonly property bool hasFullscreenShellSurfaces: {
        for (var i = 0; i < shellSurfaces.count; i++) {
            var shellSurface = shellSurfaces[i].shellSurface;
            if (shellSurface.fullscreen)
                return true;
        }

        return false;
    }

    property Component outputConfigurationComponent: OutputConfiguration {}

    readonly property alias applicationManager: applicationManager
    readonly property alias shellHelper: shellHelper
    readonly property alias policyKitAgent: policyKitAgent

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

        function createShellSurfaceItem(shellSurface, moveItem, output) {
            var parentSurfaceItem = output.viewsBySurface[shellSurface.parentSurface];
            var parent = parentSurfaceItem || output.surfacesArea;
            var item = chromeComponent.createObject(parent, {
                                                        "compositor": compositor,
                                                        "shellSurface": shellSurface,
                                                        "moveItem": moveItem
                                                    });
            output.viewsBySurface[shellSurface.surface] = item;
            return item;
        }

        function handleShellSurfaceCreated(shellSurface) {
            shellSurfaces.append({"shellSurface": shellSurface});

            var moveItem =
                    moveItemComponent.createObject(rootItem, {
                                                       "x": defaultOutput.position.x,
                                                       "y": defaultOutput.position.y,
                                                       "width": Qt.binding(function() { return shellSurface.surface.width; }),
                                                       "height": Qt.binding(function() { return shellSurface.surface.height; })
                                                   });
            for (var i = 0; i < outputs.length; i++)
                createShellSurfaceItem(shellSurface, moveItem, outputs[i]);

            applicationManager.registerShellSurface(shellSurface);
        }

        function handleShellSurfaceDestroyed(shellSurface) {
            var i;

            for (i = 0; i < shellSurfaces.count; i++) {
                if (shellSurfaces.get(i).shellSurface === shellSurface) {
                    shellSurfaces.remove(i, 1);
                    break;
                }
            }

            applicationManager.unregisterShellSurface(shellSurface);
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

    WlShell {
        onWlShellSurfaceCreated: handleShellSurfaceCreated(shellSurface)
    }

    XdgShellV5 {
        onXdgSurfaceCreated: handleShellSurfaceCreated(xdgSurface)
        onXdgPopupCreated: handleShellSurfaceCreated(xdgPopup)
    }

    GtkShell {}

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
                for (var i = 0; i < outputs.length; i++)
                    delete outputs[i].viewsBySurface[surface];
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

    // Surface item for shell helper's grab surface
    Component {
        id: grabItemComponent

        WaylandQuickItem {
            focusOnClick: false
            onSurfaceChanged: shellHelper.grabCursor(ShellHelper.ArrowGrabCursor)
        }
    }

    /*
     * Miscellaneous
     */

    // Shell helper
    ShellHelper {
        id: shellHelper
        onGrabSurfaceAdded: {
            var parentItem;
            for (var i = 0; i < __private.outputs.length; i++) {
                parentItem = __private.outputs[i].window.contentItem;
                grabItemComponent.createObject(parentItem, {"surface": surface});
            }
        }
    }

    // Holds move items in the compositor space
    Item {
        id: rootItem
    }

    // XWayland
    Loader {
        active: false
        asynchronous: true

        sourceComponent: Qt.createComponent(Qt.resolvedUrl("XWayland.qml"), Component.Asynchronous)

        onStatusChanged: {
            if (status === Loader.Error)
                console.warn("Error loading XWayland support:", sourceComponent.errorString());
        }
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

    function handleShellSurfaceCreated(shellSurface) {
        __private.handleShellSurfaceCreated(shellSurface);
    }

    function handleShellSurfaceDestroyed(shellSurface) {
        __private.handleShellSurfaceDestroyed(shellSurface);
    }

    function activateShellSurfaces(appId) {
        for (var i = 0; i < shellSurfaces.count; i++) {
            var shellSurface = shellSurfaces.get(i).shellSurface;
            if (shellSurface.canonicalAppId === appId) {
                for (var j = 0; j < __private.outputs.length; j++)
                    __private.outputs[j].activateShellSurface(shellSurface);
            }
        }
    }
}
