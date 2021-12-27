// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Aurora.Compositor 1.0
import Aurora.Compositor.Wlroots 1.0
import Aurora.Compositor.XWayland 1.0 as LXW
import Fluid.Core 1.0 as FluidCore
import Liri.private.shell 1.0 as LS

FluidCore.Object {
    id: window

    property LXW.XWaylandShellSurface shellSurface: null

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
        // Set true if it has a titlebar and decoration
        if (__private.windowType !== Qt.Popup)
            return __private.decorate && !__private.fullscreen;
        else
            return false;
    }
    readonly property bool bordered: {
        // Set true if it has resize handles
        if (decorated)
            return !__private.maximized && !__private.fullscreen;
        else
            return false;
    }
    readonly property real borderSize: bordered ? 5 : 0
    readonly property real titleBarHeight: decorated ? 32 : 0
    readonly property bool focusable: true
    readonly property bool maximizable: true
    readonly property bool resizable: !__private.maximized && !__private.fullscreen

    signal showWindowMenu(WaylandSeat seat, point localSurfacePosition)

    // Instead of binding shellSurface properties to those of the
    // window API, we have to store them into a private object to avoid issues
    // when they become null due to the surface being destroyed.
    // This is also needed for the chrome that depends on the properties here,
    // and storing the last known state prevents the chrome from doing weird
    // things for example messing the size due to a wrong window geometry value.
    QtObject {
        id: __private

        property bool registered: false
        property bool mapped: false

        property WaylandSurface surface: null
        property WaylandSurface parentSurface: null

        property int windowType
        property bool decorate
        property color backgroundColor
        property color foregroundColor
        property string title
        property rect windowGeometry
        property bool activated
        property bool maximized
        property bool fullscreen
        property bool resizing: false

        property point position
        property size size
        property point finalPosition

        function updateAppId() {
            appIdAndIcon.appId = shellSurface.appId;

            if (!__private.registered && appIdAndIcon.canonicalAppId) {
                // Register application
                applicationManager.registerShellSurface(window);
                __private.registered = true;

                // Focus icon in the panel
                if (shellSurface.activated)
                    applicationManager.focusShellSurface(window);
            }
        }

        function updateWindowGeometry() {
            __private.windowGeometry = Qt.rect(borderSize, borderSize,
                                               xdgSurface.surface.size.width,
                                               xdgSurface.surface.size.height + titleBarHeight);
        }
    }

    Component.onCompleted: {
        __private.surface = shellSurface.surface;
        __private.parentSurface = shellSurface.parentSurface;
        __private.windowType = shellSurface.windowType;
        __private.decorate = shellSurface.decorate;
        __private.backgroundColor = shellSurface.surface.backgroundColor;
        __private.foregroundColor = shellSurface.surface.foregroundColor;
        __private.title = shellSurface.title;
        __private.activated = shellSurface.activated;
        __private.maximized = shellSurface.maximized;
        __private.fullscreen = shellSurface.fullscreen;
        __private.updateAppId();
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
            if (shellSurface)
                shellSurface.maximize(liriCompositor.defaultOutput);
        }
        onUnmaximizeRequested: {
            if (shellSurface)
                shellSurface.unmaximize();
        }
        onMinimizeRequested: {
            liriCompositor.setAppMinimized(window.appId, true);
        }
        onUnminimizeRequested: {
            liriCompositor.setAppMinimized(window.appId, false);
        }
        onFullscreenRequested: {
            console.warn("Fullscreen is not supported with XWayland");
        }
        onUnfullscreenRequested: {
            console.warn("Unfullscreen is not supported with XWayland");
        }
        onActivateRequested: {
            liriCompositor.activateApp(window.appId);
        }
        onCloseRequested: {
            if (shellSurface)
                shellSurface.close();
        }
    }

    Component.onDestruction: {
        toplevelHandle.sendClosed();
    }

    Connections {
        target: __private.surface

        function onSizeChanged() {
            __private.updateWindowGeometry();
        }

        function onBackgroundColorChanged() {
            __private.backgroundColor = shellSurface.surface.backgroundColor;
        }

        function onForegroundColorChanged() {
            __private.foregroundColor = shellSurface.surface.foregroundColor;
        }
    }

    Connections {
        target: shellSurface

        function onActivatedChanged() {
            if (__private.registered && shellSurface.activated && shellSurface.windowType !== Qt.Popup)
                applicationManager.focusShellSurface(window);
        }

        function onSurfaceChanged() {
            if (shellSurface.surface) {
                __private.surface = shellSurface.surface;
                __private.backgroundColor = shellSurface.surface.backgroundColor;
                __private.foregroundColor = shellSurface.surface.foregroundColor;
            }
        }

        function onParentSurfaceChanged() {
            __private.parentSurface = shellSurface.parentSurface;
        }

        function onAppIdChanged() {
            __private.updateAppId();
        }

        function onDecorateChanged() {
            __private.decorate = shellSurface.decorate;
        }

        function onMapped() {
            __private.mapped = true;
        }

        function onUnmapped() {
            __private.mapped = false;
        }

        function onSetMinimized() {
            minimized = true;
        }
    }

    MoveItem {
        id: moveItem

        parent: rootItem
        width: windowGeometry.width
        height: windowGeometry.height
    }

    function __convertEdges(edges) {
        var wlEdges = LXW.XWaylandShellSurface.NoneEdge;
        if (edges & Qt.TopEdge && edges & Qt.LeftEdge)
            wlEdges |= LXW.XWaylandShellSurface.TopLeftEdge;
        else if (edges & Qt.BottomEdge && edges & Qt.LeftEdge)
            wlEdges |= LXW.XWaylandShellSurface.BottomLeftEdge;
        else if (edges & Qt.TopEdge && edges & Qt.RightEdge)
            wlEdges |= LXW.XWaylandShellSurface.TopRightEdge;
        else if (edges & Qt.BottomEdge && edges & Qt.RightEdge)
            wlEdges |= LXW.XWaylandShellSurface.BottomRightEdge;
        else {
            if (edges & Qt.TopEdge)
                wlEdges |= LXW.XWaylandShellSurface.TopEdge;
            if (edges & Qt.BottomEdge)
                wlEdges |= LXW.XWaylandShellSurface.BottomEdge;
            if (edges & Qt.LeftEdge)
                wlEdges |= LXW.XWaylandShellSurface.LeftEdge;
            if (edges & Qt.RightEdge)
                wlEdges |= LXW.XWaylandShellSurface.RightEdge;
        }
        return wlEdges;
    }

    function sizeForResize(size, delta, edges) {
        return shellSurface.sizeForResize(size, delta, __convertEdges(edges));
    }

    function sendResizing(size) {
        shellSurface.sendResize(size);
    }

    function sendMaximized(output) {
        shellSurface.maximize(output);
    }

    function sendUnmaximized() {
        shellSurface.unmaximize();
    }

    function sendFullscreen(output) {
        console.warn("Not implemented yet");
    }

    function pingClient() {
        console.warn("Not implemented yet");
    }

    function sendClose() {
        shellSurface.close();
    }
}
