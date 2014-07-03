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

WaylandSurfaceItem {
    property var role: surface.windowProperties.role
    property var requestedPosition: surface.windowProperties.position

    id: waylandWindow
    opacity: 1.0
    touchEventsEnabled: true
    visible: true
    onRoleChanged: {
        if (typeof(role) == "undefined")
            return;

        // Default parent
        var windowParent = getCurrentWorkspaceItem();

        // Find parent based on role
        switch (role) {
        case WaylandCompositor.CursorRole:
            windowParent = root.layers.cursor;
            break;
        case WaylandCompositor.LockScreenRole:
            windowParent = root.layers.lock;
            break;
        case WaylandCompositor.OverlayRole:
            windowParent = root.layers.overlay;
            break;
        case WaylandCompositor.DialogRole:
            windowParent = root.layers.dialogs;
            break;
        case WaylandCompositor.FullScreenRole:
            windowParent = root.layers.fullScreen;
            break;
        case WaylandCompositor.PanelRole:
            windowParent = root.layers.panels;
            break;
        case WaylandCompositor.PopupRole:
            windowParent = root.layers.panels;
            break;
        case WaylandCompositor.NotificationRole:
            windowParent = root.layers.notifications;
            break;
        case WaylandCompositor.DesktopRole:
            windowParent = root.layers.desktop;
            break;
        case WaylandCompositor.BackgroundRole:
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
        switch (role) {
        case WaylandCompositor.ApplicationRole:
            // Move application window
            surface.pos = compositor.initialPositionForSurface(surface);
            break;
        case WaylandCompositor.DialogRole:
            // Center globally modal dialogs and show overlay
            surface.pos.x = root.layers.dialogs.overlay.x + (root.layers.dialogs.overlay.width - width) / 2;
            surface.pos.y = root.layers.dialogs.overlay.y + (root.layers.dialogs.overlay.height - height) / 2;
            root.layers.dialogs.overlay.opacity = 1.0;
            break;
        default:
            break;
        }
    }
    onRequestedPositionChanged: {
        if (typeof(requestedPosition) == "undefined")
            return;

        surface.pos.x = requestedPosition.x;
        surface.pos.y = requestedPosition.y;
    }

    SurfaceRenderer {
        anchors.fill: parent
        source: parent
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

    function getCurrentWorkspaceItem() {
        return root.layers.workspaces.currentWorkspace;
    }

    function getWorkspaceItem(index) {
        // Check whether the workspace actually exists
        var workspace = root.layers.workspaces.model.get(index);
        if (typeof(workspace) == "undefined") {
            console.warn("Invalid workspace", index);
            return null;
        }

        return workspace;
    }

    function moveToWorkspace(index) {
        // Get workspace item
        var item = getWorkspaceItem(index);
        if (!item)
            return;

        // Reparent
        waylandWindow.parent = item;
    }
}
