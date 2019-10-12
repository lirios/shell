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
import QtWayland.Compositor 1.3 as QtWayland
import Fluid.Core 1.0 as FluidCore

FluidCore.Object {
    id: window

    property QtWayland.XdgSurface xdgSurface: null
    property QtWayland.XdgPopup popup: null

    readonly property QtWayland.ShellSurface shellSurface: xdgSurface
    readonly property QtWayland.WaylandSurface surface: xdgSurface ? xdgSurface.surface : null
    readonly property QtWayland.WaylandSurface parentSurface: popup && popup.parentXdgSurface ? popup.parentXdgSurface.surface : null

    readonly property int windowType: Qt.Popup

    readonly property alias mapped: d.mapped
    readonly property alias offset: d.offset

    readonly property alias moveItem: moveItem

    readonly property rect windowGeometry: {
        if (xdgSurface) {
            if (xdgSurface.windowGeometry.width < 0 && xdgSurface.windowGeometry.height < 0)
                return Qt.rect(0, 0, xdgSurface.surface.size.width, xdgSurface.surface.size.height);
            return xdgSurface.windowGeometry;
        } else {
            return Qt.rect(0, 0, -1, -1);
        }
    }

    readonly property bool focusable: false

    onPopupChanged: {
        if (popup)
            d.offset = popup.offset;
    }

    QtObject {
        id: d

        property bool mapped: false
        property point offset
    }

    Connections {
        target: surface
        onHasContentChanged: {
            if (surface.hasContent)
                d.mapped = true;
        }
    }

    MoveItem {
        id: moveItem

        parent: rootItem
        width: windowGeometry.width
        height: windowGeometry.height
    }
}
