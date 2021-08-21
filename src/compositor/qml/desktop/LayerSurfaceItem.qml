// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtWayland.Compositor 1.15
import Liri.WaylandServer 1.0 as WS
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects

WaylandQuickItem {
    id: layerSurfaceItem

    property Item containerItem: layerSurfaceItem
    property bool honorAnchors: true

    property WS.WlrLayerSurfaceV1 layerSurface: null

    property real implicitSurfaceWidth: Math.max(0, implicitWidth)
    property real implicitSurfaceHeight: Math.max(0, implicitHeight)
    readonly property real surfaceWidth: Math.max(layerSurface.width, implicitSurfaceWidth)
    readonly property real surfaceHeight: Math.max(layerSurface.height, implicitSurfaceHeight)

    readonly property real leftMargin: layerSurface.leftMargin + additionalLeftMargin
    readonly property real topMargin: layerSurface.topMargin + additionalTopMargin
    readonly property real rightMargin: layerSurface.rightMargin + additionalRightMargin
    readonly property real bottomMargin: layerSurface.bottomMargin + additionalBottomMargin

    property real additionalLeftMargin: 0
    property real additionalTopMargin: 0
    property real additionalRightMargin: 0
    property real additionalBottomMargin: 0

    readonly property real horizontalMargin: leftMargin + rightMargin + additionalLeftMargin + additionalRightMargin
    readonly property real verticalMargin: topMargin + bottomMargin + additionalTopMargin + additionalBottomMargin

    signal createAnimationFinished()
    signal destroyAnimationFinished()

    surface: layerSurface.surface
    focusOnClick: layerSurface.keyboardInteractivity > WS.WlrLayerSurfaceV1.NoKeyboardInteractivity

    z: layerSurface && layerSurface.nameSpace === "notification" ? 100 : 0
    opacity: 0.0
    visible: layerSurface.mapped && layerSurface.configured

    onSurfaceDestroyed: {
        bufferLocked = true;
        if (layerSurface.nameSpace === "dialog")
            shrinkFadeOutAnimation.start();
        else if (layerSurface.nameSpace === "lockscreen")
            lockScreenExitAnimation.start();
        else
            destroyAnimationFinished();
    }

    onVisibleChanged: {
        if (visible) {
            if (layerSurface.nameSpace === "dialog")
                growFadeInAnimation.start();
            else if (layerSurface.nameSpace === "lockscreen")
                lockScreenEnterAnimation.start();
            else {
                opacity = 1.0;
                createAnimationFinished();
            }

            if (layerSurface.keyboardInteractivity > WS.WlrLayerSurfaceV1.NoKeyboardInteractivity)
                takeFocus();
        }
    }

    Behavior on x {
        NumberAnimation { duration: 220 }
    }

    Behavior on y {
        NumberAnimation { duration: 220 }
    }

    Behavior on anchors.leftMargin {
        NumberAnimation { duration: 220 }
    }

    Behavior on anchors.topMargin {
        NumberAnimation { duration: 220 }
    }

    Behavior on anchors.rightMargin {
        NumberAnimation { duration: 220 }
    }

    Behavior on anchors.bottomMargin {
        NumberAnimation { duration: 220 }
    }

    SequentialAnimation {
        id: growFadeInAnimation

        alwaysRunToEnd: true

        ParallelAnimation {
            NumberAnimation {
                target: layerSurfaceItem
                property: "scale"
                from: 0.9
                to: 1.0
                easing.type: Easing.OutQuint
                duration: 220
            }
            NumberAnimation {
                target: layerSurfaceItem
                property: "opacity"
                from: 0.0
                to: 1.0
                easing.type: Easing.OutCubic
                duration: 150
            }
        }

        ScriptAction {
            script: {
                createAnimationFinished();
            }
        }
    }

    SequentialAnimation {
        id: shrinkFadeOutAnimation

        alwaysRunToEnd: true

        ParallelAnimation {
            NumberAnimation {
                target: layerSurfaceItem
                property: "scale"
                from: 1.0
                to: 0.9
                easing.type: Easing.OutQuint
                duration: 220
            }
            NumberAnimation {
                target: layerSurfaceItem
                property: "opacity"
                from: 1.0
                to: 0.0
                easing.type: Easing.OutCubic
                duration: 150
            }
        }

        ScriptAction {
            script: {
                destroyAnimationFinished();
            }
        }
    }

    SequentialAnimation {
        id: lockScreenEnterAnimation

        alwaysRunToEnd: true

        SequentialAnimation {
            PauseAnimation { duration: 80 }

            ParallelAnimation {
                NumberAnimation {
                    target: layerSurfaceItem
                    property: "opacity"
                    from: 0.0
                    to: 1.0
                    easing.type: Easing.Linear
                    duration: 153
                }
                NumberAnimation {
                    target: layerSurfaceItem
                    property: "y"
                    from: -height + (height * 1.05)
                    to: 0
                    easing.type: Easing.Bezier
                    easing.bezierCurve: [0.4,0,0.2,1,1,1]
                    duration: 153
                }
            }
        }

        ScriptAction {
            script: {
                createAnimationFinished();
            }
        }
    }

    SequentialAnimation {
        id: lockScreenExitAnimation

        alwaysRunToEnd: true

        ParallelAnimation {
            NumberAnimation {
                target: layerSurfaceItem
                property: "scale"
                from: 1.0
                to: 1.1
                easing.type: Easing.InQuint
                duration: FluidControls.Units.shortDuration
            }
            NumberAnimation {
                target: layerSurfaceItem
                property: "opacity"
                from: 1.0
                to: 0.0
                easing.type: Easing.InQuad
                duration: FluidControls.Units.shortDuration
            }
        }

        ScriptAction {
            script: {
                layerSurface.close();
                destroyAnimationFinished();
            }
        }
    }

    Connections {
        target: layerSurface

        function onChanged() {
            setupAnchors();
            sendConfigure();
        }

        function onLayerChanged() {
            // Create new surface item elsewhere
            var model = layerShell.getModelForLayer(layerSurface, output);
            model.append({layerSurface: layerSurface, output: output});

            // Destroy this surface item
            bufferLocked = true;
            destroyAnimationFinished();
        }
    }

    Connections {
        target: parent

        function onChildrenChanged() {
            additionalLeftMargin = calculateLeftMargin();
            additionalTopMargin = calculateTopMargin();
            additionalRightMargin = calculateRightMargin();
            additionalBottomMargin = calculateBottomMargin();
        }
    }

    function setupAnchors() {
        // Some kind of layer surfaces should not be anchored
        if (!containerItem.honorAnchors) {
            containerItem.anchors.left = undefined;
            containerItem.anchors.right = undefined;
            containerItem.anchors.top = undefined;
            containerItem.anchors.bottom = undefined;
            containerItem.anchors.horizontalCenter = undefined;
            containerItem.anchors.verticalCenter = undefined;
            containerItem.anchors.leftMargin = 0;
            containerItem.anchors.rightMargin = 0;
            containerItem.anchors.topMargin = 0;
            containerItem.anchors.bottomMargin = 0;
            return;
        }

        var hasLeft = layerSurface.anchors & WS.WlrLayerSurfaceV1.LeftAnchor;
        var hasRight = layerSurface.anchors & WS.WlrLayerSurfaceV1.RightAnchor;
        var hasTop = layerSurface.anchors & WS.WlrLayerSurfaceV1.TopAnchor;
        var hasBottom = layerSurface.anchors & WS.WlrLayerSurfaceV1.BottomAnchor;

        if (!hasLeft && !hasRight) {
            containerItem.anchors.left = undefined;
            containerItem.anchors.right = undefined;
            containerItem.anchors.horizontalCenter = containerItem.parent.horizontalCenter;
            implicitSurfaceWidth = 0;
        } else {
            containerItem.anchors.horizontalCenter = undefined;
            containerItem.anchors.left = hasLeft ? containerItem.parent.left : undefined;
            containerItem.anchors.right = hasRight ? containerItem.parent.right : undefined;
            implicitSurfaceWidth = hasLeft && hasRight ? containerItem.parent.width : 0;
        }
        if (!hasTop && !hasBottom) {
            containerItem.anchors.top = undefined;
            containerItem.anchors.bottom = undefined;
            containerItem.anchors.verticalCenter = containerItem.parent.verticalCenter;
            implicitSurfaceHeight = 0;
        } else {
            containerItem.anchors.verticalCenter = undefined;
            containerItem.anchors.top = hasTop ? containerItem.parent.top : undefined;
            containerItem.anchors.bottom = hasBottom ? containerItem.parent.bottom : undefined;
            implicitSurfaceHeight = hasTop && hasBottom ? containerItem.parent.height : 0;
        }

        if (hasLeft)
            containerItem.anchors.leftMargin = Qt.binding(function() { return leftMargin });
        else
            containerItem.anchors.leftMargin = 0;
        if (hasRight)
            containerItem.anchors.rightMargin = Qt.binding(function() { return rightMargin });
        else
            containerItem.anchors.rightMargin = 0;
        if (hasTop)
            containerItem.anchors.topMargin = Qt.binding(function() { return topMargin });
        else
            containerItem.anchors.topMargin = 0;
        if (hasBottom)
            containerItem.anchors.bottomMargin = Qt.binding(function() { return bottomMargin });
        else
            containerItem.anchors.bottomMargin = 0;
    }

    function sendConfigure() {
        additionalLeftMargin = calculateLeftMargin();
        additionalTopMargin = calculateTopMargin();
        additionalRightMargin = calculateRightMargin();
        additionalBottomMargin = calculateBottomMargin();

        // When the size is invalid the compositor determins the size, otherwise
        // it's up to the client to determine its own size
        var newSize = Qt.size(layerSurface.width, layerSurface.height);
        if (newSize.width <= 0)
            newSize.width = implicitSurfaceWidth;
        else
            newSize.width = 0;
        if (newSize.height <= 0)
            newSize.height = implicitSurfaceHeight;
        else
            newSize.height = 0;
        console.debug("Sending configure to layer surface of scope",
                      '"' + layerSurface.nameSpace + '"',
                      "with size", newSize.width + "x" + newSize.height);
        layerSurface.sendConfigure(newSize.width, newSize.height);
    }

    function reconfigure() {
        setupAnchors();

        if (layerSurface.configured)
            sendConfigure();
    }

    function calculateLeftMargin() {
        return 0;
    }

    function calculateTopMargin() {
        return 0;
    }

    function calculateRightMargin() {
        return 0;
    }

    function calculateBottomMargin() {
        if (layerSurface.nameSpace === "notification") {
            for (var i = 0; i < parent.children.length; i++) {
                if (parent.children[i] === this)
                    return (surfaceHeight + layerSurface.bottomMargin) * i;
            }
        }

        return 0;
    }
}
