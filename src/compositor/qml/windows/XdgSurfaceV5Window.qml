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

P.XdgSurfaceV5 {
    id: xdgSurface

    readonly property alias mapped: details.mapped
    property bool minimized: false

    property string canonicalAppId: applicationManager.canonicalizeAppId(appId)
    property string iconName: "unknown"

    property bool decorated: true

    property WaylandSurface parentWlSurface: parentSurface ? parentSurface.surface : null
    property point offset: Qt.point(0, 0)

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
            applicationManager.focusShellSurface(xdgSurface);
    }
    onCanonicalAppIdChanged: {
        if (!details.registered && canonicalAppId && windowType != Qt.Popup) {
            // Register application
            applicationManager.registerShellSurface(xdgSurface);
            details.registered = true;

            // Focus icon in the panel
            if (activated)
                applicationManager.focusShellSurface(xdgSurface);
        }

        // Set icon name when appId changes
        var appIconName = applicationManager.getIconName(canonicalAppId);
        iconName = appIconName ? appIconName : "unknown";
    }
    onSetTopLevel: {
        offset.x = 0;
        offset.y = 0;
    }
    onSetTransient: {
        offset.x = (parentSurface.windowGeometry.width - windowGeometry.width) / 2;
        offset.y = (parentSurface.windowGeometry.height - windowGeometry.height) / 2;
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

    function __convertEdges(edges) {
        var xdgEdges = P.XdgSurfaceV5.NoneEdge;
        if (edges & Qt.TopEdge && edges & Qt.LeftEdge)
            xdgEdges |= P.XdgSurfaceV5.TopLeftEdge;
        else if (edges & Qt.BottomEdge && edges & Qt.LeftEdge)
            xdgEdges |= P.XdgSurfaceV5.BottomLeftEdge;
        else if (edges & Qt.TopEdge && edges & Qt.RightEdge)
            xdgEdges |= P.XdgSurfaceV5.TopRightEdge;
        else if (edges & Qt.BottomEdge && edges & Qt.RightEdge)
            xdgEdges |= P.XdgSurfaceV5.BottomRightEdge;
        else {
            if (edges & Qt.TopEdge)
                xdgEdges |= P.XdgSurfaceV5.TopEdge;
            if (edges & Qt.BottomEdge)
                xdgEdges |= P.XdgSurfaceV5.BottomEdge;
            if (edges & Qt.LeftEdge)
                xdgEdges |= P.XdgSurfaceV5.LeftEdge;
            if (edges & Qt.RightEdge)
                xdgEdges |= P.XdgSurfaceV5.RightEdge;
        }
        return xdgEdges;
    }

    function interactiveResize(size, delta, edges) {
        var xdgEdges = __convertEdges(edges);
        var newSize = xdgSurface.sizeForResize(size, delta, xdgEdges);
        xdgSurface.sendResizing(newSize);
    }

    function resize(size, edges) {
        xdgSurface.sendResizing(size);
    }

    function maximize(size) {
        sendMaximized(size);
    }

    function unmaximize(size) {
        sendUnmaximized(size);
    }

    function pingClient() {
        shell.ping(surface.client);
        pingTimer.start();
    }

    function close() {
        xdgSurface.sendClose();
    }

    Component.onDestruction: {
        __private.handleShellSurfaceDestroyed(xdgSurface);

        // Unset mapped flag which will make Chrome play the animation
        details.mapped = false;
    }
}
