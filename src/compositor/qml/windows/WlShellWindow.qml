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
import QtWayland.Compositor 1.0
import Liri.private.shell 1.0 as P

P.WlShellSurface {
    id: wlShellSurface

    readonly property alias mapped: details.mapped
    property bool activated: true
    property bool minimized: false
    property bool maximized: false
    property bool fullscreen: false

    property string canonicalAppId: applicationManager.canonicalizeAppId(className)
    property string iconName: "unknown"

    property bool decorated: true

    property WaylandSurface parentWlSurface: null
    property point offset

    readonly property alias responsive: details.responsive

    readonly property alias moveItem: moveItem

    MoveItem {
        id: moveItem

        parent: rootItem
        width: surface ? surface.width : 0
        height: surface ? surface.height : 0
    }

    onActivatedChanged: {
        if (details.registered && activated && windowType != Qt.Popup)
            applicationManager.focusShellSurface(wlShellSurface);
    }
    onCanonicalAppIdChanged: {
        if (!details.registered && canonicalAppId && windowType != Qt.Popup) {
            // Register application
            applicationManager.registerShellSurface(wlShellSurface);
            details.registered = true;

            // Focus icon in the panel
            if (activated)
                applicationManager.focusShellSurface(wlShellSurface);
        }

        // Set icon name when appId changes
        var appIconName = applicationManager.getIconName(canonicalAppId);
        iconName = appIconName ? appIconName : "unknown";
    }
    onSetDefaultToplevel: {
        decorated = true;
        maximized = false;
        fullscreen = false;
        parentWlSurface = null;
        offset.x = 0;
        offset.y = 0;
    }
    onSetPopup: {
        decorated = false;
        maximized = false;
        fullscreen = false;
        parentWlSurface = parentSurface;
        offset.x = relativeToParent.x;
        offset.y = relativeToParent.y;
    }
    onSetTransient: {
        decorated = true;
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

    function __convertEdges(edges) {
        var wlEdges = P.WlShellSurface.NoneEdge;
        if (edges & Qt.TopEdge && edges & Qt.LeftEdge)
            wlEdges |= P.WlShellSurface.TopLeftEdge;
        else if (edges & Qt.BottomEdge && edges & Qt.LeftEdge)
            wlEdges |= P.WlShellSurface.BottomLeftEdge;
        else if (edges & Qt.TopEdge && edges & Qt.RightEdge)
            wlEdges |= P.WlShellSurface.TopRightEdge;
        else if (edges & Qt.BottomEdge && edges & Qt.RightEdge)
            wlEdges |= P.WlShellSurface.BottomRightEdge;
        else {
            if (edges & Qt.TopEdge)
                wlEdges |= P.WlShellSurface.TopEdge;
            if (edges & Qt.BottomEdge)
                wlEdges |= P.WlShellSurface.BottomEdge;
            if (edges & Qt.LeftEdge)
                wlEdges |= P.WlShellSurface.LeftEdge;
            if (edges & Qt.RightEdge)
                wlEdges |= P.WlShellSurface.RightEdge;
        }
        return wlEdges;
    }

    function interactiveResize(size, delta, edges) {
        var wlEdges = __convertEdges(edges);
        var newSize = wlShellSurface.sizeForResize(size, delta, wlEdges);
        wlShellSurface.sendConfigure(newSize, wlEdges);
    }

    function resize(size, edges) {
        wlShellSurface.sendConfigure(size, __convertEdges(edges));
    }

    function pingClient() {
        wlShellSurface.ping();
        pingTimer.start();
    }

    function close() {
        console.warn("wl-shell doesn't support closing windows");
    }

    Component.onDestruction: {
        __private.handleShellSurfaceDestroyed(wlShellSurface);
    }
}
