// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

MouseArea {
    property var scaler

    property real x1: 0.0
    property real y1: 0.0
    property real x2: 0.0
    property real y2: 0.0
    property real zoom1: 1.0
    property real zoom2: 1.0
    property real min: 1.0
    property real max: 10.0

    id: zoomArea
    acceptedButtons: Qt.NoButton
    propagateComposedEvents: true
    drag.target: parent
    drag.filterChildren: true
    onWheel: {
        if (!(wheel.modifiers & liriCompositor.settings.windowActionModifier)) {
            wheel.accepted = false;
            return;
        }

        zoomArea.x1 = scaler.origin.x;
        zoomArea.y1 = scaler.origin.y;
        zoomArea.zoom1 = scaler.xScale;
        zoomArea.x2 = wheel.x;
        zoomArea.y2 = wheel.y;

        var newZoom;
        if (wheel.angleDelta.y > 0) {
            newZoom = zoomArea.zoom1 + 0.1;
            if (newZoom <= zoomArea.max)
                zoomArea.zoom2 = newZoom;
            else
                zoomArea.zoom2 = zoomArea.max;
        } else {
            newZoom = zoomArea.zoom1 - 0.1;
            if (newZoom >= zoomArea.min)
                zoomArea.zoom2 = newZoom;
            else
                zoomArea.zoom2 = zoomArea.min;
        }

        wheel.accepted = true;
    }
}
