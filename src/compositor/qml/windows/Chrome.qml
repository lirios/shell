// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Aurora.Compositor 1.0
import QtGraphicalEffects 1.15
import Fluid.Effects 1.0 as FluidEffects
import Aurora.Compositor.XWayland 1.0
import Liri.private.shell 1.0 as P

P.ChromeItem {
    id: chrome

    property QtObject window: null
    property QtObject shellSurface: null
    property WaylandOutput output: null

    property int shellSurfaceType: P.ChromeItem.WaylandShellSurface

    readonly property alias bufferLocked: __private.bufferLocked
    readonly property bool maximized: window.maximized
    readonly property bool fullscreen: window.fullscreen

    readonly property real frameMarginLeft: leftResizeHandleItem.width
    readonly property real frameMarginTop: topResizeHandleItem.height + titleBar.height
    readonly property real frameMarginRight: rightResizeHandleItem.width
    readonly property real frameMarginBottom: bottomResizeHandleItem.height

    readonly property alias shellSurfaceWidth: __private.shellSurfaceWidth
    readonly property alias shellSurfaceHeight: __private.shellSurfaceHeight

    property rect taskIconGeometry: Qt.rect(0, 0, 32, 32)

    signal activated()
    signal createAnimationFinished()
    signal destroyAnimationFinished()

    x: window.moveItem.x - output.position.x
    y: window.moveItem.y - output.position.y

    width: shellSurfaceWidth + frameMarginLeft + frameMarginRight
    height: shellSurfaceHeight + frameMarginTop + frameMarginBottom

    enabled: !chrome.output.locked && !liriCompositor.showModalOverlay

    transform: [
        Scale {
            id: scaleTransform

            origin.x: chrome.width / 2
            origin.y: chrome.height / 2
        }
    ]

    visible: false

    QtObject {
        id: __private

        property bool justCreated: true
        property bool bufferLocked: false
        property real shellSurfaceWidth: 0
        property real shellSurfaceHeight: 0
        property point moveItemPosition

        function initialSetup() {
            // Set initial position
            let parentSurfaceItem = output.viewsBySurface[window.parentSurface];
            if (parentSurfaceItem) {
                window.moveItem.x = parentSurfaceItem.window.moveItem.x + ((parentSurfaceItem.width - chrome.width) / 2);
                window.moveItem.y = parentSurfaceItem.window.moveItem.y + ((parentSurfaceItem.height - chrome.height) / 2);
            } else {
                let position = randomPosition(liriCompositor.mousePos);
                window.moveItem.x = position.x;
                window.moveItem.y = position.y;
            }

            // Now that the position is correct, make it visible
            visible = true;

            // Activate
            if (window.focusable)
                takeFocus();

            // Create animation
            if (workspace.state === "normal")
                createAnimation.start();
        }
    }

    Connections {
        target: window

        function onMappedChanged() {
            if (window.mapped)
                __private.initialSetup();
        }

        function onActivatedChanged() {
            if (__private.justCreated)
                return;

            if (window.activated) {
                raise();
                activated();
            }
        }

        function onMinimizedChanged() {
            if (window.minimized)
                minimizeAnimation.start();
            else
                unminimizeAnimation.start();
        }

        function onShowWindowMenu(seat, localSurfacePosition) {
            showWindowMenu(localSurfacePosition.x, localSurfacePosition.y);
        }
    }

    // Drop shadow
    BorderImage {
        anchors.fill: parent
        anchors.margins: -47

        z: -1

        border.left: 50
        border.top: 50
        border.right: 50
        border.bottom: 50

        // Always cache the dropshadow
        cache: true

        source: "qrc:/images/chrome/dropshadow-" + (window.activated ? "24" : "8") + ".png"

        visible: window.bordered
    }

    // Left edge for resizing
    Item {
        id: leftResizeHandleItem

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: visible ? window.borderSize : 0
        visible: window.bordered

        HoverHandler {
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.SizeHorCursor
        }

        ResizeHandler {
            id: leftResizeHandle

            edges: Qt.LeftEdge
            cursorShape: Qt.SizeHorCursor
            enabled: parent.visible && window.resizable
        }
    }

    // Top edge for resizing
    Item {
        id: topResizeHandleItem

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        height: visible ? window.borderSize : 0
        visible: window.bordered

        HoverHandler {
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.SizeVerCursor
        }

        ResizeHandler {
            id: topResizeHandle

            edges: Qt.TopEdge
            cursorShape: Qt.SizeVerCursor
            enabled: parent.visible && window.resizable
        }
    }

    // Right edge for resizing
    Item {
        id: rightResizeHandleItem

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: visible ? window.borderSize : 0
        visible: window.bordered

        HoverHandler {
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.SizeHorCursor
        }

        ResizeHandler {
            id: rightResizeHandle

            edges: Qt.RightEdge
            cursorShape: Qt.SizeHorCursor
            enabled: parent.visible && window.resizable
        }
    }

    // Bottom edge for resizing
    Item {
        id: bottomResizeHandleItem

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: visible ? window.borderSize : 0
        visible: window.bordered

        HoverHandler {
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.SizeVerCursor
        }

        ResizeHandler {
            id: bottomResizeHandle

            edges: Qt.BottomEdge
            cursorShape: Qt.SizeVerCursor
            enabled: parent.visible && window.resizable
        }
    }

    // Top left corner for resizing
    Item {
        id: topLeftResizeHandleItem

        anchors.left: parent.left
        anchors.top: parent.top
        width: visible ? window.borderSize : 0
        height: visible ? window.borderSize : 0
        visible: window.bordered

        HoverHandler {
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.SizeFDiagCursor
        }

        ResizeHandler {
            id: topLeftResizeHandle

            edges: Qt.TopEdge | Qt.LeftEdge
            cursorShape: Qt.SizeFDiagCursor
            enabled: parent.visible && window.resizable
        }
    }

    // Top right corner for resizing
    Item {
        id: topRightResizeHandleItem

        anchors.top: parent.top
        anchors.right: parent.right
        width: visible ? window.borderSize : 0
        height: visible ? window.borderSize : 0
        visible: window.bordered

        HoverHandler {
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.SizeBDiagCursor
        }

        ResizeHandler {
            id: topRightResizeHandle

            edges: Qt.TopEdge | Qt.RightEdge
            cursorShape: Qt.SizeBDiagCursor
            enabled: parent.visible && window.resizable
        }
    }

    // Bottom left corner for resizing
    Item {
        id: bottomLeftResizeHandleItem

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: visible ? window.borderSize : 0
        height: visible ? window.borderSize : 0
        visible: window.bordered

        HoverHandler {
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.SizeBDiagCursor
        }

        ResizeHandler {
            id: bottomLeftResizeHandle

            edges: Qt.BottomEdge | Qt.LeftEdge
            cursorShape: Qt.SizeBDiagCursor
            enabled: parent.visible && window.resizable
        }
    }

    // Bottom right corner for resizing
    Item {
        id: bottomRightResizeHandleItem

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: visible ? window.borderSize : 0
        height: visible ? window.borderSize : 0
        visible: window.bordered

        HoverHandler {
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.SizeFDiagCursor
        }

        ResizeHandler {
            id: bottomRightResizeHandle

            edges: Qt.BottomEdge | Qt.RightEdge
            cursorShape: Qt.SizeFDiagCursor
            enabled: parent.visible && window.resizable
        }
    }

    // Container
    Item {
        anchors.left: leftResizeHandleItem.right
        anchors.top: topResizeHandleItem.bottom
        anchors.right: rightResizeHandleItem.left
        anchors.bottom: bottomResizeHandleItem.top

        // Rounded corners
        layer.enabled: window.bordered
        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: shellSurfaceWidth
                height: shellSurfaceHeight
                radius: 4
            }
        }

        // Title and window controls
        TitleBar {
            id: titleBar

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right
            iconName: window.iconName
            title: window.title
            backgroundColor: window.backgroundColor
            foregroundColor: window.foregroundColor
            activated: window.activated
            maximizable: window.maximizable
            maximized: window.maximized
            dragTarget: window.moveItem
            height: visible ? window.titleBarHeight : 0
            visible: window.decorated

            Binding {
                target: window.moveItem
                property: "moving"
                value: titleBar.moving
            }

            onActivationRequested: {
                if (window.focusable)
                    shellSurfaceItem.takeFocus();
            }
            onMinimizeClicked: {
                window.minimized = true;
            }
            onMaximizeClicked: {
                if (window.maximized)
                    window.sendUnmaximized();
                else
                    window.sendMaximized(output);
            }
            onCloseClicked: {
                __private.bufferLocked = true;
                window.sendClose();
            }
            onWindowMenuRequested: {
                showWindowMenu(position.x, position.y);
            }
        }

        // Actual window contents for Wayland shell surfaces
        Loader {
            id: waylandShellSurfaceItemLoader

            anchors.left: parent.left
            anchors.top: titleBar.bottom

            active: shellSurfaceType == P.ChromeItem.WaylandShellSurface
            sourceComponent: ShellSurfaceItem {
                output: chrome.output
                shellSurface: chrome.shellSurface
                autoCreatePopupItems: true
                moveItem: window.moveItem
                focusOnClick: window.focusable
                bufferLocked: chrome.bufferLocked

                onSurfaceChanged: {
                    if (output && surface)
                        output.viewsBySurface[surface] = chrome;
                }
                onSurfaceDestroyed: {
                    bufferLocked = true;
                    destroyAnimation.start();
                }
            }

            onLoaded: {
                shellSurfaceItem = item;
                __private.shellSurfaceWidth = Qt.binding(function() { return item.width });
                __private.shellSurfaceHeight = Qt.binding(function() { return item.height });
            }
        }

        // Actual window contents for XWayland surfaces
        Loader {
            id: xwaylandShellSurfaceItemLoader

            anchors.left: parent.left
            anchors.top: titleBar.bottom

            active: shellSurfaceType == P.ChromeItem.XWaylandShellSurface
            sourceComponent: XWaylandShellSurfaceItem {
                output: chrome.output
                shellSurface: chrome.shellSurface
                moveItem: window.moveItem
                focusOnClick: window.focusable
                bufferLocked: chrome.bufferLocked

                onSurfaceChanged: {
                    if (output && surface)
                        output.viewsBySurface[surface] = chrome;
                }
                onSurfaceDestroyed: {
                    bufferLocked = true;
                    if (chrome.window.windowType === Qt.Popup)
                        popupDestroyAnimation.start();
                    else
                        destroyAnimation.start();
                }
            }

            onLoaded: {
                shellSurfaceItem = item;
                __private.shellSurfaceWidth = Qt.binding(function() { return item.width });
                __private.shellSurfaceHeight = Qt.binding(function() { return item.height });
            }
        }
    }

    ChromeMenu {
        id: chromeMenu
    }

    // Drag windows with Meta
    DragHandler {
        id: dragHandler

        property var movingBinding: Binding {
            target: dragHandler.target
            property: "moving"
            value: dragHandler.active
        }

        acceptedModifiers: liriCompositor.settings.windowActionModifier
        target: window.moveItem
        enabled: !liriCompositor.showModalOverlay
    }

    // Animate the creation of this window
    SequentialAnimation {
        id: createAnimation

        alwaysRunToEnd: true

        ParallelAnimation {
            NumberAnimation {
                target: scaleTransform
                property: "xScale"
                from: 0
                to: 1
                duration: 150
            }
            NumberAnimation {
                target: scaleTransform
                property: "yScale"
                from: 0
                to: 1
                duration: 150
            }
            NumberAnimation {
                target: chrome
                property: "opacity"
                from: 0.0
                to: 1.0
                duration: 150
            }
        }

        ScriptAction {
            script: {
                chrome.createAnimationFinished();
                __private.justCreated = false;
            }
        }
    }

    // Animate the destruction of this window
    SequentialAnimation {
        id: destroyAnimation

        alwaysRunToEnd: true

        ParallelAnimation {
            NumberAnimation {
                target: scaleTransform
                property: "xScale"
                to: 4 / chrome.width
                duration: 150
            }
            NumberAnimation {
                target: scaleTransform
                property: "yScale"
                to: 4 / chrome.height
                duration: 150
            }
            NumberAnimation {
                target: chrome
                property: "opacity"
                to: 0.0
                duration: 150
            }
        }

        ScriptAction {
            script: {
                // Give focus back to the parent
                var parentSurfaceItem = output.viewsBySurface[window.parentSurface];
                if (parentSurfaceItem)
                    parentSurfaceItem.takeFocus();

                // We are done with the animation
                chrome.destroyAnimationFinished();
            }
        }
    }

    // Animate the window when it's minimized
    SequentialAnimation {
        id: minimizeAnimation

        alwaysRunToEnd: true

        ScriptAction {
            script: {
                __private.moveItemPosition.x = window.moveItem.x;
                __private.moveItemPosition.y = window.moveItem.y;
            }
        }

        ParallelAnimation {
            SmoothedAnimation {
                target: window.moveItem
                property: "x"
                duration: 250
                to: taskIconGeometry.x + (taskIconGeometry.width / 2) - (chrome.width / 2)
            }
            SmoothedAnimation {
                target: window.moveItem
                property: "y"
                duration: 250
                to: taskIconGeometry.y + (taskIconGeometry.height / 2)
            }
            NumberAnimation {
                target: chrome
                property: "scale"
                easing.type: Easing.OutQuad
                to: 0.25
                duration: 250
            }
            NumberAnimation {
                target: chrome
                property: "opacity"
                easing.type: Easing.Linear
                to: 0.0
                duration: 250
            }
        }
    }

    // Animate the window when it's restored from the taskbar
    SequentialAnimation {
        id: unminimizeAnimation

        alwaysRunToEnd: true

        ParallelAnimation {
            SmoothedAnimation {
                target: window.moveItem
                property: "x"
                duration: 200
                to: __private.moveItemPosition.x
            }
            SmoothedAnimation {
                target: window.moveItem
                property: "y"
                duration: 200
                to: __private.moveItemPosition.y
            }
            NumberAnimation {
                target: chrome
                property: "scale"
                easing.type: Easing.OutQuad
                to: 1.0
                duration: 200
            }
            NumberAnimation {
                target: chrome
                property: "opacity"
                easing.type: Easing.Linear
                to: 1.0
                duration: 200
            }
        }
    }

    /*
     * Methods
     */

    function showWindowMenu(x, y) {
        chromeMenu.x = x;
        chromeMenu.y = y;
        chromeMenu.open();
    }
}
