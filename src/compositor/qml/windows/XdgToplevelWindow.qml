// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtWayland.Compositor 1.3 as QtWayland
import Fluid.Core 1.0 as FluidCore
import Liri.WaylandServer 1.0 as WS
import Liri.private.shell 1.0 as LS

LS.WaylandWindow {
    id: window

    property QtWayland.XdgSurface xdgSurface: null
    property QtWayland.XdgToplevel toplevel: null

    readonly property QtWayland.ShellSurface shellSurface: xdgSurface
    readonly property QtWayland.WaylandSurface surface: xdgSurface ? xdgSurface.surface : null
    readonly property alias parentSurface: d.parentSurface

    readonly property alias windowType: d.windowType

    readonly property alias mapped: d.mapped

    readonly property alias moveItem: moveItem

    readonly property string title: toplevel ? toplevel.title : ""

    readonly property rect windowGeometry: {
        var rect = Qt.rect(0, 0, 0, 0);
        if (xdgSurface) {
            if (xdgSurface.windowGeometry.width <= 0 && xdgSurface.windowGeometry.height <= 0)
                rect = Qt.rect(0, 0, xdgSurface.surface.size.width, xdgSurface.surface.size.height);
            else
                rect = xdgSurface.windowGeometry;
        }

        if (decorated)
            return Qt.rect(borderSize, borderSize, rect.width + borderSize, rect.height + borderSize + titleBarHeight);
        return rect;
    }
    readonly property rect surfaceGeometry: {
        var rect = Qt.rect(0, 0, 0, 0);
        if (xdgSurface) {
            if (xdgSurface.windowGeometry.width <= 0 && xdgSurface.windowGeometry.height <= 0)
                rect = Qt.rect(0, 0, xdgSurface.surface.size.width, xdgSurface.surface.size.height);
            else
                rect = xdgSurface.windowGeometry;
        }
        return rect;
    }

    readonly property size minSize: toplevel ? toplevel.minSize : Qt.size(0, 0)
    readonly property size maxSize: toplevel ? toplevel.maxSize : Qt.size(0, 0)

    readonly property bool activated: toplevel && toplevel.activated
    property bool minimized: false
    readonly property bool maximized: toplevel && toplevel.maximized
    readonly property bool fullscreen: toplevel && toplevel.fullscreen
    readonly property bool resizing: toplevel && toplevel.resizing

    readonly property bool decorated: {
        if (toplevel) {
            if (toplevel.decorationMode === QtWayland.XdgToplevel.ServerSideDecoration)
                return !toplevel.fullscreen;
        }

        return false;
    }
    readonly property bool bordered: {
        if (toplevel) {
            if (toplevel.decorationMode === QtWayland.XdgToplevel.ServerSideDecoration)
                return !toplevel.maximized && !toplevel.fullscreen;
        }

        return false;
    }
    readonly property real borderSize: bordered ? 4 : 0
    readonly property real titleBarHeight: decorated ? 32 : 0

    readonly property bool focusable: true
    readonly property bool maximizable: {
        if (minSize.width <= 0 && minSize.height <= 0 && maxSize.width <= 0 && maxSize.height <= 0)
            return true;
        return minSize !== maxSize;
    }

    signal showWindowMenu(QtWayland.WaylandSeat seat, point localSurfacePosition)

    QtObject {
        id: d

        property int windowType: Qt.Window
        property bool mapped: false
        property bool registered: false
        property QtWayland.WaylandSurface parentSurface: null
        property point position
        property size size
        property point finalPosition
    }

    WS.WlrForeignToplevelHandleV1 {
        id: toplevelHandle

        compositor: liriCompositor
        maximized: window.maximized
        minimized: window.minimized
        activated: window.activated
        fullscreen: window.fullscreen
        title: window.title
        appId: window.appId
        onMaximizeRequested: {
            if (toplevel)
                toplevel.sendMaximized(liriCompositor.defaultOutput.availableGeometry.size);
        }
        onUnmaximizeRequested: {
            if (toplevel)
                toplevel.sendUnmaximized();
        }
        onMinimizeRequested: {
            liriCompositor.setAppMinimized(window.appId, true);
        }
        onUnminimizeRequested: {
            liriCompositor.setAppMinimized(window.appId, false);
        }
        onFullscreenRequested: {
            var outputToUse = output ? output : liriCompositor.defaultOutput;
            if (toplevel)
                toplevel.sendFullscreen(outputToUse.geometry.size);
        }
        onUnfullscreenRequested: {
            console.warn("Unfullscreen is not supported with xdg-shell");
        }
        onActivateRequested: {
            liriCompositor.activateApp(window.appId);
        }
        onCloseRequested: {
            if (toplevel)
                toplevel.sendClose();
        }
    }

    Component.onDestruction: {
        toplevelHandle.sendClosed();
    }

    Connections {
        target: surface

        function onHasContentChanged() {
            if (surface.hasContent)
                d.mapped = true;
        }
        function onRedraw() {
            if (!window.decorated)
                return;

            if (d.finalPosition.x !== -1 && d.finalPosition.y !== -1) {
                moveItem.x = d.finalPosition.x;
                moveItem.y = d.finalPosition.y;
                d.finalPosition = Qt.point(-1, -1);
            }
        }
    }

    Connections {
        target: toplevel

        function onActivatedChanged() {
            if (d.registered && toplevel.activated)
                applicationManager.focusShellSurface(window);
        }
        function onAppIdChanged() {
            // Canonicalize app id and cache it, so that it's known even during destruction
            window.appId = applicationManager.canonicalizeAppId(toplevel.appId);

            if (!d.registered && window.appId) {
                // Register application
                applicationManager.registerShellSurface(window);
                d.registered = true;

                // Focus icon in the panel
                if (toplevel.activated)
                    applicationManager.focusShellSurface(window);
            }
        }
        function onStartMove(seat) {
            // TODO: Set move cursor
        }
        function onSetMinimized() {
            window.minimized = true;
        }
        function onShowWindowMenu() {
            window.showWindowMenu(seat, localSurfacePosition);
        }
        function onParentToplevelChanged() {
            if (toplevel && toplevel.parentToplevel) {
                d.parentSurface = null;
                for (var i = 0; i < liriCompositor.shellSurfaces.count; i++) {
                    var currentWindow = liriCompositor.shellSurfaces.get(i).window;
                    if (currentWindow.toplevel === toplevel.parentToplevel) {
                        d.parentSurface = currentWindow.surface;
                        break;
                    }
                }
                d.windowType = Qt.SubWindow;
            } else {
                d.parentSurface = null;
                d.windowType = Qt.Window;
            }
        }
    }

    MoveItem {
        id: moveItem

        parent: rootItem
        width: windowGeometry.width
        height: windowGeometry.height
    }

    function sizeForResize(size, delta, edges) {
        return toplevel.sizeForResize(size, delta, edges);
    }

    function sendResizing(size) {
        toplevel.sendResizing(size);
    }

    function sendMaximized(output) {
        // Save position and size but only if it's the first time
        if (!toplevel.maximized) {
            d.position = Qt.point(moveItem.x, moveItem.y);
            d.size = Qt.size(windowGeometry.width, windowGeometry.height);
        }

        var w = output.availableGeometry.width;
        var h = output.availableGeometry.height - titleBarHeight;

        d.finalPosition = Qt.point(output.position.x + output.availableGeometry.x,
                                   output.position.y + output.availableGeometry.y);

        toplevel.sendMaximized(Qt.size(w, h));
    }

    function sendUnmaximized() {
        if (!toplevel.maximized)
            return;

        d.finalPosition = d.position;

        toplevel.sendUnmaximized(d.size);
    }

    function sendFullscreen(output) {
        // Save position and size but only if it's the first time
        if (!toplevel.fullscreen) {
            d.position = Qt.point(moveItem.x, moveItem.y);
            d.size = Qt.size(windowGeometry.width, windowGeometry.height);
        }

        var w = output.geometry.width;
        var h = output.geometry.height;

        d.finalPosition = output.position;

        toplevel.sendFullscreen(Qt.size(w, h));
    }

    function sendClose() {
        toplevel.sendClose();
    }
}
