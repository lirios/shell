/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import QtGraphicalEffects 1.0
import WaylandCompositor 1.0
import GreenIsland 1.0
import Hawaii.Shell 0.1

WaylandSurfaceItem {
    id: surfaceItem
    onSurfaceChanged: {
        // Render the surface as soon as it is available
        renderer.source = surfaceItem;

        // Handle surface properties
        if (surfaceItem.surface) {
            surfaceProperties.target = surfaceItem.surface.windowProperties;
            surfaceItem.role = surfaceItem.surface.windowProperties.role;
            surfaceItem.restack();
        } else {
            surfaceProperties.target = null;
        }
    }

    property bool animationsEnabled: true
    property var role: null

    // Render item taking care of y inverted surfaces
    SurfaceRenderer {
        id: renderer
        anchors.fill: surfaceItem
    }

    // React to window properties changes
    Connections {
        id: surfaceProperties
        onRoleChanged: {
            surfaceItem.role = surfaceItem.surface.windowProperties.role;
            surfaceItem.restack();
        }
    }

    function restack() {
        if (surfaceItem.role == null)
            return;

        switch (surfaceItem.role) {
        case Compositor.BackgroundWindowRole:
            surfaceItem.z = 0;
            break;
        case Compositor.PanelWindowRole:
        case Compositor.LauncherWindowRole:
        case Compositor.SpecialWindowRole:
        case Compositor.PopupWindowRole:
            surfaceItem.z = 2;
            break;
        case Compositor.OverlayWindowRole:
            surfaceItem.z = 3;
            break;
        case Compositor.DialogWindowRole:
            surfaceItem.z = 4;
            break;
        default:
            break;
        }
    }
}
