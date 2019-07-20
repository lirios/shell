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
import QtWayland.Compositor 1.3
import Liri.WaylandServer 1.0 as WS

WaylandQuickItem {
    property var layerSurface: undefined
    property bool configured: false

    anchors {
        top: layerSurface.anchors & WS.WlrLayerSurfaceV1.TopAnchor
             ? parent.top : undefined
        bottom: layerSurface.anchors & WS.WlrLayerSurfaceV1.BottomAnchor
                ? parent.bottom : undefined
        left: layerSurface.anchors & WS.WlrLayerSurfaceV1.LeftAnchor
              ? parent.left : undefined
        right: layerSurface.anchors & WS.WlrLayerSurfaceV1.RightAnchor
               ? parent.right : undefined
        leftMargin: layerSurface.leftMargin
        rightMargin: layerSurface.rightMargin
        topMargin: layerSurface.topMargin
        bottomMargin: layerSurface.bottomMargin
    }

    focusOnClick: layerSurface.keyboardInteractivity
    sizeFollowsSurface: !(layerSurface.width === 0 && layerSurface.height === 0)

    onWidthChanged: {
        if (!configured)
            sendConfigure();
    }
    onHeightChanged: {
        if (!configured)
            sendConfigure();
    }

    function sendConfigure() {
        if (!sizeFollowsSurface && width > 0 && height > 0) {
            layerSurface.sendConfigure(width, height);
            configured = true;
        }
    }
}
