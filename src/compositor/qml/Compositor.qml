// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml
import QtQuick
import QtQuick.Window
import Aurora.Compositor
import Aurora.Compositor.Ext
import Aurora.Compositor.Liri
import Aurora.Compositor.Wlroots
import Aurora.Compositor.WlrLayerShell
import Aurora.Compositor.XdgShell
import Aurora.Compositor.XWayland as LXW
import Aurora.Platform
import Liri.Session as Session
import Liri.private.shell as P
import Liri.Shell.Compositor as LS
import "desktop"
import "windows"

WaylandCompositor {
    id: liriCompositor

    property point mousePos: Qt.point(0, 0)

    readonly property alias idleInhibit: __private.idleInhibit

    readonly property alias settings: settings

    readonly property alias windows: __private.windows

    readonly property alias showModalOverlay: liriModal.grabbed

    readonly property alias shellHelper: shellHelper

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

            Session.SessionManager.setEnvironment("WAYLAND_DISPLAY", liriCompositor.socketName);
            SessionInterface.registerService();

            if (xwayland.enabled)
                xwayland.startServer();
        }
    }

    onSurfaceRequested: (client, id, version) => {
        var surface = surfaceComponent.createObject(liriCompositor, {});
        surface.initialize(liriCompositor, client, id, version);
    }

    Connections {
        target: Session.SessionManager

        function onSessionLocked() {
            // Run the session locker process
            sessionLockerProcess.start();
        }

        function onIdleInhibitRequested() {
            __private.idleInhibit++;
        }

        function onIdleUninhibitRequested() {
            __private.idleInhibit--;
        }
    }

    QtObject {
        id: __private

        property int idleInhibit: 0

        property var windows: []
    }

    /*
     * Output management
     */

    PlatformOutputsModel {
        id: platformOutputsModel
    }

    Instantiator {
        model: platformOutputsModel
        delegate: Output {
            compositor: liriCompositor
            platformOutput: output

            Component.onCompleted: {
                // Set this as default output if configured
                if (!liriCompositor.defaultOutput && output.name === settings.outputs.primary)
                    liriCompositor.defaultOutput = this;

                // Fallback to the first one
                if (!liriCompositor.defaultOutput)
                    liriCompositor.defaultOutput = this;
            }
        }

        onObjectRemoved: {
            // Move all windows that fit entirely the removed output to the primary output,
            // unless the output remove is the primary one (this shouldn't happen)
            if (object === liriCompositor.defaultOutput)
                return;
            for (var surface in object.viewsBySurface) {
                var view = object.viewsBySurface[surface];
                if (view.primary && view.output === object) {
                    view.window.moveItem.x = liriCompositor.defaultOutput.position.x + 20;
                    view.window.moveItem.y = liriCompositor.defaultOutput.position.y + 20;
                }
            }
        }
    }

    /*
     * Extensions
     */

    QtWindowManager {
        showIsFullScreen: false
        onOpenUrl: Session.Launcher.launchCommand("xdg-open %1".arg(url))
    }

    // Liri shell

    LS.HelperLauncher {
        id: shellProcess

        helper: LS.HelperLauncher.Shell
        socketName: liriCompositor.socketName
        running: liriCompositor.created
    }

    LS.HelperLauncher {
        id: sessionLockerProcess

        helper: LS.HelperLauncher.SessionLocker
        socketName: liriCompositor.socketName
    }

    LS.LiriShellV1 {
        id: shellHelper

        property bool isReady: false

        onShortcutBound: (shortcut) => {
            shortcutComponent.incubateObject(keyBindings, { shortcut: shortcut });
        }
        onReady: {
            isReady = true;
            shellHelperTimer.running = false;

            for (var i = 0; i < outputs.length; i++)
                outputs[i].reveal();
        }
        onTerminateRequested: {
            liriCompositor.quit();
        }
    }

    LS.LiriModalManagerV1 {
        id: liriModal
    }

    Timer {
        id: shellHelperTimer

        interval: 15000
        running: true
        onTriggered: {
            for (var i = 0; i < outputs.length; i++)
                outputs[i].reveal();
        }
    }

    LS.LiriOsdV1 {
        id: liriOsd
    }

    Component {
        id: shortcutComponent

        Shortcut {
            property LS.LiriShortcutV1 shortcut: null

            context: Qt.ApplicationShortcut
            sequence: shortcut ? shortcut.sequence : ""
            enabled: shellHelper.isReady
            onActivated: {
                shortcut.activate();
            }
        }
    }

    // Session lock manager

    ExtSessionLockManagerV1 {
        id: sessionLockManager

        focusPolicy: ExtSessionLockManagerV1.AutomaticFocus

        onLockSurfaceCreated: {
            if (lockSurface.output)
                lockSurface.output.lockSurfacesModel.append({lockSurface: lockSurface});
        }
    }

    // Layer shell

    WlrLayerShellV1 {
        id: layerShell

        onLayerSurfaceCreated: (layerSurface) => {
            var output = layerSurface.output;
            if (!output)
                output = liriCompositor.defaultOutput;
            if (output)
                output.layerSurfacesModel.append({layerSurface: layerSurface, output});
        }
    }

    // Decorations

    XdgDecorationManagerV1 {
        preferredMode: settings.ui.clientSideDecoration ? XdgToplevel.ClientSideDecoration : XdgToplevel.ServerSideDecoration
    }

    FluidDecorationManagerV1 {
        onDecorationCreated: (decoration) => {
            decoration.foregroundColorChanged.connect(function(color) {
                decoration.surface.foregroundColor = color;
            });
            decoration.backgroundColorChanged.connect(function(color) {
                decoration.surface.backgroundColor = color;
            });
        }
    }

    // Foreign toplevel management

    WlrForeignToplevelManagerV1 {
        id: foreignToplevelManager
    }

    // Screen copy

    WlrExportDmabufManagerV1 {
        onOutputCaptureRequested: {
            if (frame.output.screen) {
                frame.output.exportDmabufFrame = frame;
                screenCast.enable(frame.output.screen.screen);
            }
        }
    }

    WlrScreencopyManagerV1 {
        onCaptureOutputRequested: {
            frame.ready.connect(function() {
                frame.copy("desktop");
                liriCompositor.flash();
            });
        }
    }

    LS.LiriColorPickerManagerV1 {
        layerName: "desktop"
    }

    // Shells

    Component {
        id: xdgToplevelComponent

        XdgToplevelWindow {
            id: window

            Component.onDestruction: {
                // Remove from the list of xdg-shell toplevels
                var toplevelIndex = xdgShell.toplevels.indexOf(toplevel);
                if (toplevelIndex >= 0)
                    xdgShell.toplevels.splice(toplevelIndex, 1);

                // Remove from the list of windows
                var windowIndex = windows.indexOf(window);
                if (windowIndex >= 0)
                    windows.splice(windowIndex, 1);
            }
        }
    }

    XdgShell {
        id: xdgShell

        property var toplevels: ([])

        onToplevelCreated: (toplevel, xdgSurface) => {
            var window = xdgToplevelComponent.createObject(xdgShell, {xdgSurface: xdgSurface, toplevel: toplevel});
            for (var i = 0; i < outputs.length; i++)
                outputs[i].currentWorkspace.shellSurfaces.append({shellSurface: xdgSurface, window: window, output: outputs[i]});
            toplevels.push(toplevel);
            windows.push(window);
        }
    }

    Component {
        id: xwaylandWindowComponent

        XWaylandWindow {
            id: window

            Component.onDestruction: {
                // Remove from the list of windows
                var windowIndex = windows.indexOf(window);
                if (windowIndex >= 0)
                    windows.splice(windowIndex, 1);
            }
        }
    }

    Component {
        id: shellSurfaceComponent

        LXW.XWaylandShellSurface {}
    }

    LXW.XWayland {
        id: xwayland

        enabled: liriCompositor.settings.shell.enableXwayland
        manager: LXW.XWaylandManager {
            id: manager

            onShellSurfaceRequested: {
                var shellSurface = shellSurfaceComponent.createObject(manager);
                shellSurface.initialize(manager, window, geometry, overrideRedirect, parentShellSurface);
            }
            onShellSurfaceCreated: {
                var window = xwaylandWindowComponent.createObject(manager, {shellSurface: shellSurface});
                for (var i = 0; i < outputs.length; i++)
                    outputs[i].currentWorkspace.xwaylandShellSurfaces.append({shellSurface: shellSurface, window: window, output: outputs[i]});
                windows.push(window);
            }
        }
        onServerStarted: (displayName) => {
            console.info("Xwayland server started");
            Session.SessionManager.setEnvironment("DISPLAY", displayName);
        }
    }

    // Text input

    TextInputManager {}

    /*
     * D-Bus
     */

    P.MultimediaKeysServer {
        id: multimediakeysServer
    }

    P.OsdServer {
        id: osdServer

        onTextRequested: {
            liriOsd.showText(iconName, text);
        }
        onProgressRequested: {
            liriOsd.showProgress(iconName, value);
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

            property color foregroundColor: "transparent"
            property color backgroundColor: "transparent"

            Component.onDestruction: {
                for (var i = 0; i < outputs.length; i++)
                    delete outputs[i].viewsBySurface[surface];
            }
        }
    }

    /*
     * Miscellaneous
     */

    // Holds move items in the compositor space
    Item {
        id: rootItem
    }

    ShellSettings {
        id: settings
    }

    KeyBindings {
        id: keyBindings
    }

    Timer {
        id: idleTimer

        interval: settings.session.idleDelay * 1000
        running: true
        repeat: true
        onTriggered: {
            var i, output, idleHint = false;
            for (i = 0; i < outputs.length; i++) {
                output = outputs[i];
                if (idleInhibit + output.idleInhibit == 0) {
                    output.idle();
                    idleHint = true;
                }
            }

            Session.SessionManager.idle = idleHint;
        }
    }

    /*
     * Methods
     */

    function wake() {
        var i;
        for (i = 0; i < outputs.length; i++) {
            idleTimer.restart();
            outputs[i].wake();
        }

        Session.SessionManager.idle = false;
    }

    function idle() {
        var i;
        for (i = 0; i < outputs.length; i++)
            outputs[i].idle();

        Session.SessionManager.idle = true;
    }

    function flash() {
        var i;
        for (i = 0; i < outputs.length; i++)
            outputs[i].flash();
    }

    function activateApp(appId) {
        for (var i = 0; i < windows.length; i++) {
            if (windows[i].appId === appId) {
                windows[i].minimized = false;
                for (var j = 0; j < outputs.length; j++)
                    outputs[j].viewsBySurface[windows[i].surface].takeFocus();
            }
        }
    }

    function setAppMinimized(appId, minimized) {
        for (var i = 0; i < windows.length; i++) {
            if (windows[i].appId === appId)
                windows[i].minimized = minimized;
        }
    }

    function quit() {
        layerShell.closeAllLayerSurfaces();
        shellHelper.sendQuit();

        for (var i = 0; i < outputs.length; i++)
            outputs[i].window.close();

        Qt.quit();
    }
}
