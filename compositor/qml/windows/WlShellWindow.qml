/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Liri.WaylandServer.Private 1.0 as LWSP

LWSP.WlShellSurface {
    id: wlShellSurface

    readonly property alias mapped: details.mapped
    property bool activated: true
    property bool minimized: false
    property bool maximized: false
    property bool fullscreen: false
    property string canonicalAppId: applicationManager.canonicalizeAppId(className)

    property bool decorated: false
    property bool hasDropShadow: !maximized && !fullscreen

    property WaylandSurface parentWlSurface: null
    property point offset

    readonly property alias responsive: details.responsive

    readonly property alias moveItem: moveItem

    MoveItem {
        id: moveItem

        parent: rootItem
        width: surface.width
        height: surface.height
    }

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

    function maximize(output) {
        wlShellSurface.setMaximized(output);
    }

    function unmaximize() {
        if (windowType == Qt.Window)
            wlShellSurface.setDefaultToplevel();
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
