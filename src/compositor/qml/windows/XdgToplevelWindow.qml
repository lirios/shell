// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Aurora.Compositor 1.0
import Aurora.Compositor.Wlroots 1.0
import Aurora.Compositor.XdgShell 1.0
import Fluid.Core 1.0 as FluidCore
import Liri.Shell.Compositor 1.0 as LS

FluidCore.Object {
    id: window

    property XdgSurface xdgSurface: null
    property XdgToplevel toplevel: null

    readonly property alias surface: __private.surface
    readonly property alias parentSurface: __private.parentSurface
    readonly property alias mapped: __private.mapped
    readonly property alias backgroundColor: __private.backgroundColor
    readonly property alias foregroundColor: __private.foregroundColor
    readonly property alias moveItem: moveItem
    readonly property alias appId: appIdAndIcon.canonicalAppId
    readonly property alias iconName: appIdAndIcon.iconName
    readonly property alias title: __private.title
    readonly property alias windowGeometry: __private.windowGeometry
    readonly property alias activated: __private.activated
    property bool minimized: false
    readonly property alias maximized: __private.maximized
    readonly property alias fullscreen: __private.fullscreen
    readonly property alias resizing: __private.resizing
    readonly property bool decorated: {
        // Set true if it has a titlebar and decorations
        if (__private.decorationMode === XdgToplevel.ServerSideDecoration)
            return !__private.fullscreen;
        else
            return false;
    }
    readonly property bool bordered: {
        // Set true if it has resize handles
        if (__private.decorationMode === XdgToplevel.ServerSideDecoration)
            return !__private.maximized && !__private.fullscreen;
        else
            return false;
    }
    readonly property real borderSize: bordered ? 5 : 0
    readonly property real titleBarHeight: decorated ? 32 : 0
    readonly property bool focusable: true
    readonly property bool maximizable: {
        if (__private.minSize.width <= 0 && __private.minSize.height <= 0 && __private.maxSize.width <= 0 && __private.maxSize.height <= 0)
            return true;
        return __private.minSize !== __private.maxSize;
    }
    readonly property bool resizable: !__private.maximized && !__private.fullscreen

    signal showWindowMenu(WaylandSeat seat, point localSurfacePosition)

    // Instead of binding xdgSurface and toplevel properties to those of the
    // window API, we have to store them into a private object to avoid issues
    // when they become null due to the surface being destroyed.
    // This is also needed for the chrome that depends on the properties here,
    // and storing the last known state prevents the chrome from doing weird
    // things for example messing the size due to a wrong window geometry value.
    QtObject {
        id: __private

        property bool mapped: false

        property WaylandSurface surface: null
        property WaylandSurface parentSurface: null

        property int decorationMode
        property color backgroundColor
        property color foregroundColor
        property string title
        property rect windowGeometry
        property size minSize
        property size maxSize
        property bool activated
        property bool maximized
        property bool fullscreen
        property bool resizing: false

        property point position
        property size size
        property point finalPosition

        function updateAppId() {
            appIdAndIcon.appId = toplevel.appId;
        }

        function updateWindowGeometry() {
            if (__private.decorationMode === XdgToplevel.ServerSideDecoration)
                __private.windowGeometry = Qt.rect(borderSize, borderSize,
                                                   xdgSurface.surface.destinationSize.width,
                                                   xdgSurface.surface.destinationSize.height + titleBarHeight);
            else
                __private.windowGeometry = xdgSurface.windowGeometry;
        }

        function updateParentToplevel() {
            if (toplevel.parentToplevel)
                __private.parentSurface = toplevel.parentToplevel.xdgSurface.surface;
            else
                __private.parentSurface = null;
        }
    }

    Component.onCompleted: {
        __private.surface = xdgSurface.surface;
        __private.decorationMode = toplevel.decorationMode;
        __private.backgroundColor = xdgSurface.surface.backgroundColor;
        __private.foregroundColor = xdgSurface.surface.foregroundColor;
        __private.title = toplevel.title;
        __private.minSize = toplevel.minSize;
        __private.maxSize = toplevel.maxSize;
        __private.activated = toplevel.activated;
        __private.maximized = toplevel.maximized;
        __private.fullscreen = toplevel.fullscreen;
        __private.updateAppId();
        __private.updateParentToplevel();
        __private.updateWindowGeometry();
    }

    LS.AppIdAndIcon {
        id: appIdAndIcon
    }

    WlrForeignToplevelHandleV1 {
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
        target: __private.surface

        function onDestinationSizeChanged() {
            __private.updateWindowGeometry();
        }

        function onHasContentChanged() {
            if (xdgSurface && xdgSurface.surface && xdgSurface.surface.hasContent)
                __private.mapped = true;
        }

        function onRedraw() {
            if (!window.decorated)
                return;

            if (__private.finalPosition.x !== -1 && __private.finalPosition.y !== -1) {
                moveItem.x = __private.finalPosition.x;
                moveItem.y = __private.finalPosition.y;
                __private.finalPosition = Qt.point(-1, -1);
            }
        }

        function onBackgroundColorChanged() {
            __private.backgroundColor = xdgSurface.surface.backgroundColor;
        }

        function onForegroundColorChanged() {
            __private.foregroundColor = xdgSurface.surface.foregroundColor;
        }
    }

    Connections {
        target: xdgSurface

        function onSurfaceChanged() {
            if (xdgSurface.surface) {
                __private.surface = xdgSurface.surface;
                __private.backgroundColor = xdgSurface.surface.backgroundColor;
                __private.foregroundColor = xdgSurface.surface.foregroundColor;
            }
        }

        function onWindowGeometryChanged() {
            __private.updateWindowGeometry();
        }
    }

    Connections {
        target: toplevel

        function onAppIdChanged() {
            __private.updateAppId();
        }

        function onTitleChanged() {
            __private.title = toplevel.title;
        }

        function onMinSizeChanged() {
            __private.minSize = toplevel.minSize;
        }

        function onMaxSizeChanged() {
            __private.maxSize = toplevel.maxSize;
        }

        function onActivatedChanged() {
            __private.activated = toplevel.activated;
        }

        function onMaximizedChanged() {
            __private.maximized = toplevel.maximized;
        }

        function onFullscreenChanged() {
            __private.fullscreen = toplevel.fullscreen;
        }

        function onResizingChanged() {
            __private.resizing = toplevel.resizing;
        }

        function onDecorationModeChanged() {
            __private.decorationMode = toplevel.decorationMode;
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
            __private.updateParentToplevel();
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
            __private.position = Qt.point(moveItem.x, moveItem.y);
            __private.size = Qt.size(windowGeometry.width, windowGeometry.height);
        }

        var w = output.availableGeometry.width;
        var h = output.availableGeometry.height - titleBarHeight;

        __private.finalPosition = Qt.point(output.position.x + output.availableGeometry.x,
                                   output.position.y + output.availableGeometry.y);

        toplevel.sendMaximized(Qt.size(w, h));
    }

    function sendUnmaximized() {
        if (!toplevel.maximized)
            return;

        __private.finalPosition = __private.position;

        toplevel.sendUnmaximized(__private.size);
    }

    function sendFullscreen(output) {
        // Save position and size but only if it's the first time
        if (!toplevel.fullscreen) {
            __private.position = Qt.point(moveItem.x, moveItem.y);
            __private.size = Qt.size(windowGeometry.width, windowGeometry.height);
        }

        var w = output.geometry.width;
        var h = output.geometry.height;

        __private.finalPosition = output.position;

        toplevel.sendFullscreen(Qt.size(w, h));
    }

    function sendClose() {
        toplevel.sendClose();
    }
}
