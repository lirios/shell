/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import WaylandCompositor 1.0
import GreenIsland 1.0

Item {
    id: wrapper
    width: window.width
    height: window.height
    onVisibleChanged: window.clientRenderingEnabled = visible

    property WaylandSurfaceItem window
    property int role: window.surface.windowProperties.role

    // Render item taking care of y inverted surfaces
    SurfaceRenderer {
        id: renderer
    }

    Component.onCompleted: {
        // Reparent the actual surface item
        window.parent = wrapper;

        // Setup surface renderer
        renderer.anchors.fill = window;
        renderer.source = window;
    }
}
