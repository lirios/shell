/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

    property QtWayland.XdgSurfaceV6 xdgSurface: null
    property QtWayland.XdgToplevelV6 toplevel: null

    readonly property QtWayland.ShellSurface shellSurface: xdgSurface
    readonly property QtWayland.WaylandSurface surface: xdgSurface ? xdgSurface.surface : null
    readonly property alias parentSurface: d.parentSurface

    readonly property alias windowType: d.windowType

    readonly property alias mapped: d.mapped

    readonly property alias moveItem: moveItem

    readonly property string title: toplevel ? toplevel.title : ""
    readonly property string appId: d.appId
    readonly property string iconName: d.iconName

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

    readonly property alias decorated: d.decorated
    readonly property alias bordered: d.bordered
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
        property string appId: ""
        property string iconName: ""
        property bool mapped: false
        property bool registered: false
        property bool decorated: true
        property bool bordered: true
        property QtWayland.WaylandSurface parentSurface: null
        property point position
        property size size
        property point finalPosition

        function resetDecoration() {
            if (window.surface) {
                if (window.surface.decorationMode !== WS.KdeServerDecorationManager.Client) {
                    if (window.toplevel)
                        d.decorated = !window.toplevel.fullscreen;
                } else {
                    d.decorated = false;
                }
            }

            if (window.toplevel)
                d.bordered = d.decorated && !window.toplevel.maximized;
        }
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
            console.warn("Unfullscreen is not supported with xdg-shell-v6");
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
        onHasContentChanged: {
            if (surface.hasContent)
                d.mapped = true;
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
        target: toplevel
        onActivatedChanged: {
            if (d.registered && toplevel.activated)
                applicationManager.focusShellSurface(window);
        }
        onAppIdChanged: {
            // Canonicalize app id and cache it, so that it's known even during destruction
            d.appId = applicationManager.canonicalizeAppId(toplevel.appId);

            if (!d.registered && d.appId) {
                // Register application
                applicationManager.registerShellSurface(window);
                d.registered = true;

                // Focus icon in the panel
                if (toplevel.activated)
                    applicationManager.focusShellSurface(window);
            }

            // Set icon name when appId changes
            var appIconName = applicationManager.getIconName(d.appId);
            d.iconName = appIconName ? appIconName : "";
        }
        onStartMove: {
            shellHelper.grabCursor(WS.LiriShell.MoveGrabCursor);
        }
        onSetMinimized: {
            window.minimized = true;
        }
        onMaximizedChanged: {
            d.resetDecoration();
        }
        onFullscreenChanged: {
            d.resetDecoration();
        }
        onShowWindowMenu: {
            window.showWindowMenu(seat, localSurfacePosition);
        }
        onParentToplevelChanged: {
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
