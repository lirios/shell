/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
import QtCompositor 1.0
import GreenIsland.Core 1.0

Item {
    property var child
    property var role: child.surface.windowProperties.role
    property var requestedPosition: child.surface.windowProperties.position

    id: waylandWindow
    opacity: 1.0
    onVisibleChanged: {
        if (child)
            child.surface.clientRenderingEnabled = visible;
    }
    /*
    onRequestedPositionChanged: {
        if (typeof(requestedPosition) == "undefined")
            return;

        surface.pos.x = requestedPosition.x;
        surface.pos.y = requestedPosition.y;
    }
    */

    SurfaceRenderer {
        anchors.fill: parent
        source: child
    }

    Connections {
        target: child.surface
        onSizeChanged: {
            waylandWindow.width = child.surface.size.width;
            waylandWindow.height = child.surface.size.height;
        }
    }

    MouseArea {
        anchors.fill: parent
        enabled: !waylandWindow.focus
        preventStealing: false
        onClicked: {
            // Change stacking order
            var i, windowList = waylandWindow.parent.children;
            for (i = windowList.length; i >= 0; i--) {
                var curWindow = windowList[i];

                if (curWindow !== waylandWindow)
                    curWindow.z = -i;
            }
            waylandWindow.z = 1;

            // Give window focus
            waylandWindow.takeFocus();
        }
    }

    function reparent() {
        if (!child || typeof(role) == "undefined")
            return;

        // Default parent
        var windowParent = getCurrentWorkspaceItem();

        // Find parent based on role
        switch (role) {
        case Compositor.CursorRole:
            windowParent = root.layers.cursor;
            break;
        case Compositor.LockScreenRole:
            windowParent = root.layers.lock;
            break;
        case Compositor.OverlayRole:
            windowParent = root.layers.overlay;
            break;
        case Compositor.DialogRole:
            windowParent = root.layers.dialogs;
            break;
        case Compositor.FullScreenRole:
            windowParent = root.layers.fullScreen;
            break;
        case Compositor.PanelRole:
            windowParent = root.layers.panels;
            break;
        case Compositor.PopupRole:
            windowParent = root.layers.panels;
            break;
        case Compositor.NotificationRole:
            windowParent = root.layers.notifications;
            break;
        case Compositor.DesktopRole:
            windowParent = root.layers.desktop;
            break;
        case Compositor.BackgroundRole:
            windowParent = root.layers.background;
            break;
        default:
            break;
        }

        // Reparent
        if (windowParent)
            parent = windowParent;
        else
            console.warn("Invalid parent for surface with role", role);

        // Change window position if needed
        /*
        switch (role) {
        case Compositor.ApplicationRole:
            // Move application window
            surface.pos = compositor.initialPositionForSurface(surface);
            break;
        case Compositor.DialogRole:
            // Center globally modal dialogs and show overlay
            surface.pos.x = root.layers.dialogs.overlay.x + (root.layers.dialogs.overlay.width - width) / 2;
            surface.pos.y = root.layers.dialogs.overlay.y + (root.layers.dialogs.overlay.height - height) / 2;
            root.layers.dialogs.overlay.opacity = 1.0;
            break;
        default:
            break;
        }
        */
    }

    function getCurrentWorkspaceItem() {
        return root.layers.workspaces.get(root.layers.workspaces.currentWorkspaceIndex);
    }
}
