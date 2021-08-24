// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

DragHandler {
    property int edges: 0
    property bool resizing: false
    property point initialPosition: Qt.point(-1, -1)
    property size initialSize: Qt.size(0, 0)
    property size lastSize: Qt.size(0, 0)

    target: null
    grabPermissions: DragHandler.CanTakeOverFromHandlersOfSameType
    enabled: shellSurfaceItem.inputEventsEnabled && visible
    onGrabChanged: {
        resizing = false;
    }
    onCentroidChanged: {
        if (!active)
            return;

        if (!resizing) {
            initialPosition = centroid.scenePosition;
            initialSize = Qt.size(shellSurfaceWidth, shellSurfaceHeight);
            lastSize = initialSize;
            resizing = true;
        }

        var deltaX = centroid.scenePosition.x - initialPosition.x;
        var deltaY = centroid.scenePosition.y - initialPosition.y;
        var delta = Qt.point(deltaX, deltaY);
        var newSize = chrome.window.sizeForResize(initialSize, delta, edges);
        chrome.window.sendResizing(newSize);

        if (edges & Qt.LeftEdge)
            chrome.window.moveItem.x += lastSize.width - newSize.width;
        if (edges & Qt.TopEdge)
            chrome.window.moveItem.y += lastSize.height - newSize.height;

        lastSize = newSize;
    }
}
