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
    property real implicitSurfaceWidth: implicitWidth
    property real implicitSurfaceHeight: implicitHeight
    property real surfaceWidth: layerSurface.width > 0 ? layerSurface.width : implicitSurfaceWidth
    property real surfaceHeight: layerSurface.height > 0 ? layerSurface.height : implicitSurfaceHeight
    property bool configured: false

    anchors {
        leftMargin: layerSurface.leftMargin
        rightMargin: layerSurface.rightMargin
        topMargin: layerSurface.topMargin
        bottomMargin: layerSurface.bottomMargin
    }

    paintEnabled: configured
    focusOnClick: layerSurface.keyboardInteractivity

    Connections {
        target: layerSurface
        onChanged: {
            setupAnchors();
            sendConfigure();
        }
        onAnchorsChanged: {
            setupAnchors();

            if (configured) {
                configured = false;
                sendConfigure();
            }
        }
    }

    function setupAnchors() {
        var hasLeft = layerSurface.anchors & WS.WlrLayerSurfaceV1.LeftAnchor == WS.WlrLayerSurfaceV1.LeftAnchor;
        var hasRight = layerSurface.anchors & WS.WlrLayerSurfaceV1.RightAnchor == WS.WlrLayerSurfaceV1.RightAnchor;
        var hasTop = layerSurface.anchors & WS.WlrLayerSurfaceV1.TopAnchor == WS.WlrLayerSurfaceV1.TopAnchor;
        var hasBottom = layerSurface.anchors & WS.WlrLayerSurfaceV1.BottomAnchor == WS.WlrLayerSurfaceV1.BottomAnchor;

        if (!hasLeft && !hasRight) {
            anchors.left = undefined;
            anchors.right = undefined;
            anchors.horizontalCenter = parent.horizontalCenter;
            implicitSurfaceWidth = -1;
        } else {
            anchors.horizontalCenter = undefined;
            anchors.left = hasLeft ? parent.left : undefined;
            anchors.right = hasRight ? parent.right : undefined;
            implicitSurfaceWidth = hasLeft && hasRight ? parent.width : -1;
        }
        if (!hasTop && !hasBottom) {
            anchors.top = undefined;
            anchors.bottom = undefined;
            anchors.verticalCenter = parent.verticalCenter;
            implicitSurfaceHeight = -1;
        } else {
            anchors.verticalCenter = undefined;
            anchors.top = hasTop ? parent.top : undefined;
            anchors.bottom = hasBottom ? parent.bottom : undefined;
            implicitSurfaceHeight = hasTop && hasBottom ? parent.height : -1;
        }
    }

    function sendConfigure() {
        if (configured)
            return;

        console.debug("Layer surface", layerSurface.nameSpace, "size:", surfaceWidth + "x" + surfaceHeight);
        if (surfaceWidth > 0 && surfaceHeight > 0) {
            layerSurface.sendConfigure(surfaceWidth, surfaceHeight);
            configured = true;
        }
    }
}
