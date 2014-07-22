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

WaylandWindow {
    property var requestedPosition: child.surface.windowProperties.position

    id: shellWindow
    objectName: "shellWindow"
    onRequestedPositionChanged: {
        if (typeof(requestedPosition) == "undefined")
            return;

        child.surface.pos.x = requestedPosition.x;
        child.surface.pos.y = requestedPosition.y;
    }

    function reparent() {
        if (!child || typeof(role) == "undefined")
            return;

        var screenView = compositorRoot.screenViews.screenViewForCoordinates(shellWindow.x, shellWindow.y);
        var windowParent;

        // Find parent based on role
        switch (role) {
        case Compositor.CursorRole:
            windowParent = screenView.layers.cursor;
            break;
        case Compositor.LockScreenRole:
            windowParent = screenView.layers.lock;
            break;
        case Compositor.OverlayRole:
            windowParent = screenView.layers.overlay;
            break;
        case Compositor.DialogRole:
            windowParent = screenView.layers.dialogs;
            break;
        case Compositor.FullScreenRole:
            windowParent = screenView.layers.fullScreen;
            break;
        case Compositor.PanelRole:
            windowParent = screenView.layers.panels;
            break;
        case Compositor.PopupRole:
            windowParent = screenView.layers.panels;
            break;
        case Compositor.NotificationRole:
            windowParent = screenView.layers.notifications;
            break;
        case Compositor.DesktopRole:
            windowParent = screenView.layers.desktop;
            break;
        case Compositor.BackgroundRole:
            windowParent = screenView.layers.background;
            break;
        default:
            console.error("Unrecognized surface role for shell window");
            return;
        }

        // Reparent
        if (windowParent)
            parent = windowParent;
        else
            console.warn("Invalid parent for surface with role", role);

        // Change window position if needed
        /*
        switch (role) {
        case Compositor.DialogRole:
            // Center globally modal dialogs and show overlay
            child.surface.pos.x = compositorRoot.layers.dialogs.overlay.x + (compositorRoot.layers.dialogs.overlay.width - width) / 2;
            child.surface.pos.y = compositorRoot.layers.dialogs.overlay.y + (compositorRoot.layers.dialogs.overlay.height - height) / 2;
            compositorRoot.layers.dialogs.overlay.opacity = 1.0;
            break;
        default:
            break;
        }
        */
    }
}
