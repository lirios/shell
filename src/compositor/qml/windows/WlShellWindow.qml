/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import QtWayland.Compositor 1.1 as QtWayland
import Fluid.Core 1.0 as FluidCore
import Liri.WaylandServer 1.0 as WS

FluidCore.Object {
    id: window

    property QtWayland.WlShellSurface shellSurface: null
    readonly property QtWayland.WaylandSurface surface: shellSurface ? shellSurface.surface : null
    readonly property alias parentSurface: d.parentSurface

    readonly property alias windowType: d.windowType

    readonly property alias mapped: d.mapped

    readonly property alias moveItem: moveItem

    readonly property string title: shellSurface ? shellSurface.title : ""
    readonly property alias appId: d.appId
    readonly property alias iconName: d.iconName

    readonly property alias windowGeometry: d.windowGeometry
    readonly property alias surfaceGeometry: d.surfaceGeometry

    readonly property size minSize: Qt.size(0, 0)
    readonly property size maxSize: Qt.size(0, 0)

    readonly property alias activated: d.activated
    property bool minimized: false
    readonly property alias maximized: d.maximized
    readonly property alias fullscreen: d.fullscreen
    readonly property bool resizing: false

    readonly property alias decorated: d.decorated
    readonly property alias bordered: d.bordered
    readonly property real borderSize: bordered ? 4 : 0
    readonly property real titleBarHeight: decorated ? 32 : 0

    readonly property bool focusable: d.windowType !== Qt.Popup
    readonly property bool maximizable: true

    signal showWindowMenu(QtWayland.WaylandSeat seat, point localSurfacePosition)

    QtObject {
        id: d

        property int windowType: Qt.Window
        property string appId: ""
        property string iconName: ""
        property rect windowGeometry
        property rect surfaceGeometry
        property bool mapped: false
        property bool registered: false
        property bool decorated: true
        property bool bordered: true
        property bool activated: false
        property bool maximized: false
        property bool fullscreen: false
        property bool responsive: true
        property QtWayland.WaylandSurface parentSurface: null
        property point position
        property size size
        property point finalPosition

        onActivatedChanged: {
            if (d.registered && d.activated)
                applicationManager.focusShellSurface(window);
        }
        onMaximizedChanged: {
            d.resetDecoration();
        }
        onFullscreenChanged: {
            d.resetDecoration();
        }

        function recalculateWindowGeometry() {
            var w = window.surface ? window.surface.size.width : 0;
            var h = window.surface ? window.surface.size.height : 0;

            if (window.decorated)
                return Qt.rect(window.borderSize, window.borderSize,
                               w + window.borderSize, h + window.borderSize + window.titleBarHeight);

            return Qt.rect(0, 0, w > 0 ? w : -1, h > 0 ? h : -1);
        }

        function recalculateSurfaceGeometry() {
            var w = window.surface ? window.surface.size.width : 0;
            var h = window.surface ? window.surface.size.height : 0;

            return Qt.rect(0, 0, w > 0 ? w : -1, h > 0 ? h : -1);
        }

        function resetDecoration() {
            if (window.surface) {
                if (window.surface.decorationMode !== WS.KdeServerDecorationManager.Client)
                    d.decorated = !d.fullscreen;
                else
                    d.decorated = false;
            }

            d.bordered = d.decorated && !d.maximized;
        }
    }

    Connections {
        target: defaultSeat
        onKeyboardFocusChanged: d.activated = newFocus == surface
    }

    Connections {
        target: surface
        onHasContentChanged: {
            if (surface.hasContent)
                d.mapped = true;
        }
        onSizeChanged: {
            d.windowGeometry = d.recalculateWindowGeometry();
            d.surfaceGeometry = d.recalculateSurfaceGeometry();
        }
        onRedraw: {
            if (!window.decorated)
                return;

            if (d.finalPosition.x !== -1 && d.finalPosition.y !== -1) {
                moveItem.x = d.finalPosition.x;
                moveItem.y = d.finalPosition.y;
                d.finalPosition = Qt.point(-1, -1);
            }
        }
        onDecorationModeChanged: {
            d.resetDecoration();
        }
    }

    Connections {
        target: shellSurface
        onClassNameChanged: {
            // Canonicalize app id and cache it, so that it's known even during destruction
            d.appId = applicationManager.canonicalizeAppId(shellSurface.className);

            if (!d.registered && d.appId) {
                // Register application
                applicationManager.registerShellSurface(window);
                d.registered = true;

                // Focus icon in the panel
                if (d.activated)
                    applicationManager.focusShellSurface(window);
            }

            // Set icon name when appId changes
            var appIconName = applicationManager.getIconName(d.appId);
            d.iconName = appIconName ? appIconName : "";
        }
        onStartMove: {
            shellHelper.grabCursor(WS.LiriShell.MoveGrabCursor);
        }
        onSetDefaultToplevel: {
            d.maximized = false;
            d.fullscreen = false;
            d.parentSurface = null;
            d.windowType = Qt.Window;
        }
        onSetPopup: {
            d.maximized = false;
            d.fullscreen = false;
            d.parentSurface = parentSurface;
            d.windowType = Qt.Popup;
        }
        onSetTransient: {
            d.maximized = false;
            d.fullscreen = false;
            d.parentSurface = parentSurface;
            d.windowType = Qt.SubWindow;
        }
        onSetMaximized: {
            d.maximized = true;
        }
        onSetFullScreen: {
            d.fullscreen = true;
        }
        onPong: {
            pingTimer.stop();
            d.responsive = true;
        }
    }

    MoveItem {
        id: moveItem

        parent: rootItem
        width: windowGeometry.width
        height: windowGeometry.height
    }

    Timer {
        id: pingTimer
        interval: 500
        onTriggered: d.responsive = false
    }

    function __convertEdges(edges) {
        var wlEdges = QtWayland.WlShellSurface.NoneEdge;
        if (edges & Qt.TopEdge && edges & Qt.LeftEdge)
            wlEdges |= QtWayland.WlShellSurface.TopLeftEdge;
        else if (edges & Qt.BottomEdge && edges & Qt.LeftEdge)
            wlEdges |= QtWayland.WlShellSurface.BottomLeftEdge;
        else if (edges & Qt.TopEdge && edges & Qt.RightEdge)
            wlEdges |= QtWayland.WlShellSurface.TopRightEdge;
        else if (edges & Qt.BottomEdge && edges & Qt.RightEdge)
            wlEdges |= QtWayland.WlShellSurface.BottomRightEdge;
        else {
            if (edges & Qt.TopEdge)
                wlEdges |= QtWayland.WlShellSurface.TopEdge;
            if (edges & Qt.BottomEdge)
                wlEdges |= QtWayland.WlShellSurface.BottomEdge;
            if (edges & Qt.LeftEdge)
                wlEdges |= QtWayland.WlShellSurface.LeftEdge;
            if (edges & Qt.RightEdge)
                wlEdges |= QtWayland.WlShellSurface.RightEdge;
        }
        return wlEdges;
    }

    function sizeForResize(size, delta, edges) {
        var wlEdges = __convertEdges(edges);
        return shellSurface.sizeForResize(size, delta, edges);
    }

    function sendResizing(size) {
        shellSurface.sendConfigure(size, QtWayland.WlShellSurface.NoneEdge);
    }

    function sendMaximized(output) {
        // Save position and size but only if it's the first time
        if (!d.maximized) {
            d.position = Qt.point(moveItem.x, moveItem.y);
            d.size = Qt.size(windowGeometry.width, windowGeometry.height);
        }

        var w = output.availableGeometry.width;
        var h = output.availableGeometry.height - titleBarHeight;

        d.finalPosition = Qt.point(output.position.x + output.availableGeometry.x,
                                   output.position.y + output.availableGeometry.y);

        d.sendConfigure(Qt.size(w, h), QtWayland.WlShellSurface.NoneEdge);
    }

    function sendUnmaximized() {
        if (!d.maximized)
            return;

        d.finalPosition = d.position;

        d.sendConfigure(d.size, QtWayland.WlShellSurface.NoneEdge);
        d.maximized = false;
    }

    function sendFullscreen(output) {
        // Save position and size but only if it's the first time
        if (!d.fullscreen) {
            d.position = Qt.point(moveItem.x, moveItem.y);
            d.size = Qt.size(windowGeometry.width, windowGeometry.height);
        }

        var w = output.geometry.width;
        var h = output.geometry.height;

        d.finalPosition = output.position;

        d.sendConfigure(Qt.size(w, h), QtWayland.WlShellSurface.NoneEdge);
    }

    function pingClient() {
        shellSurface.ping();
        pingTimer.start();
    }

    function sendClose() {
        console.warn("wl-shell doesn't support closing windows");
    }
}
