// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls
import Liri.WaylandServer 1.0 as WS

MouseArea {
    id: decoration

    readonly property alias drag: moveArea.drag

    property color foregroundColor: Material.primaryTextColor
    property color backgroundColor: Material.color(Material.Blue)

    Material.theme: Material.Dark

    QtObject {
        id: d

        property bool resizing: false
        property int edges: 0
        property point initialPos
        property size initialSize
    }

    acceptedButtons: Qt.LeftButton

    hoverEnabled: enabled
    scrollGestureEnabled: false

    onPressed: {
        d.resizing = true;
        d.edges = 0;
        d.initialPos.x = mouse.x;
        d.initialPos.y = mouse.y;
        d.initialSize.width = shellSurfaceItem.width;
        d.initialSize.height = shellSurfaceItem.height;

        if (mouse.x <= chrome.window.borderSize)
            d.edges |= Qt.LeftEdge;
        else if (mouse.x >= this.width - chrome.window.borderSize)
            d.edges |= Qt.RightEdge;
        if (mouse.y <= chrome.window.borderSize)
            d.edges |= Qt.TopEdge;
        else if (mouse.y >= this.height - chrome.window.borderSize)
            d.edges |= Qt.BottomEdge;

        // Focus on click
        if (shellSurfaceItem.focusOnClick && mouse.button === Qt.LeftButton)
            shellSurfaceItem.takeFocus();
    }
    onPositionChanged: {
        if (pressed) {
            var delta = Qt.point(mouse.x - d.initialPos.x, mouse.y - d.initialPos.y);
            var newSize = chrome.window.sizeForResize(d.initialSize, delta, d.edges);
            chrome.window.sendResizing(newSize);
        } else {
            d.resizing = false;
        }

        // Draw cursor
        var edges = 0;

        if (mouse.x <= chrome.window.borderSize)
            edges |= Qt.LeftEdge;
        else if (mouse.x >= this.width - chrome.window.borderSize)
            edges |= Qt.RightEdge;
        if (mouse.y <= chrome.window.borderSize)
            edges |= Qt.TopEdge;
        else if (mouse.y >= this.height - chrome.window.borderSize)
            edges |= Qt.BottomEdge;

        if (edges & Qt.TopEdge && edges & Qt.LeftEdge)
            cursorShape = Qt.SizeFDiagCursor;
        else if (edges & Qt.BottomEdge && edges & Qt.LeftEdge)
            cursorShape = Qt.SizeBDiagCursor;
        else if (edges & Qt.TopEdge && edges & Qt.RightEdge)
            cursorShape = Qt.SizeFDiagCursor;
        else if (edges & Qt.BottomEdge && edges & Qt.RightEdge)
            cursorShape = Qt.SizeFDiagCursor;
        else {
            if (edges & Qt.LeftEdge)
                cursorShape = Qt.SizeHorCursor;
            else if (edges & Qt.RightEdge)
                cursorShape = Qt.SizeHorCursor;
            if (edges & Qt.TopEdge)
                cursorShape = Qt.SizeVerCursor;
            else if (edges & Qt.BottomEdge)
                cursorShape = Qt.SizeVerCursor;
        }
    }

    Connections {
        target: chrome.window.surface

        function onSizeChanged() {
            if (d.resizing) {
                var dx = 0;
                var dy = 0;

                if (d.edges & Qt.LeftEdge)
                    dx += d.initialSize.width - shellSurfaceItem.width;
                if (d.edges & Qt.TopEdge)
                    dy += d.initialSize.height - shellSurfaceItem.height;

                chrome.window.moveItem.x += dx;
                chrome.window.moveItem.y += dy;
            }
        }
        function onForegroundColorChanged() {
            decoration.foregroundColor = chrome.window.surface.foregroundColor;
        }
        function onBackgroundColorChanged() {
            decoration.backgroundColor = chrome.window.surface.backgroundColor;
        }
    }

    Rectangle {
        id: titleBar

        x: chrome.window.borderSize + chrome.window.surfaceGeometry.x
        y: chrome.window.borderSize + chrome.window.surfaceGeometry.y

        implicitWidth: shellSurfaceItem.width
        implicitHeight: 32

        color: backgroundColor

        Item {
            anchors.fill: parent
            anchors.bottomMargin: parent.radius
            opacity: chrome.window.activated ? 1.0 : 0.5

            FluidControls.Icon {
                id: icon

                anchors {
                    left: parent.left
                    leftMargin: 8
                    verticalCenter: parent.verticalCenter
                }

                name: chrome.window.iconName ? chrome.window.iconName : ""
                width: 24
                height: width
                visible: name != "" && status == Image.Ready
            }

            Label {
                id: titleBarLabel

                anchors {
                    left: icon.visible ? icon.right : parent.left
                    right: windowControls.left
                    verticalCenter: parent.verticalCenter
                }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                font.pixelSize: 16
                color: foregroundColor
                text: chrome.window.title ? chrome.window.title : ""
                wrapMode: Text.NoWrap
            }

            MouseArea {
                id: moveArea

                anchors {
                    left: parent.left
                    top: parent.top
                    right: windowControls.left
                    bottom: parent.bottom
                    bottomMargin: parent.radius
                }

                acceptedButtons: Qt.LeftButton | Qt.RightButton

                enabled: decoration.enabled
                hoverEnabled: decoration.hoverEnabled
                scrollGestureEnabled: false

                onPressed: {
                    if (shellSurfaceItem.focusOnClick && mouse.button === Qt.LeftButton)
                        shellSurfaceItem.takeFocus();
                }
                onReleased: {
                    cursorShape = Qt.ArrowCursor;

                    if (mouse.button === Qt.RightButton)
                        chrome.window.showWindowMenu(compositor.defaultSeat, Qt.point(mouse.x, mouse.y));
                }
                onEntered: {
                    cursorShape = Qt.OpenHandCursor;
                }
                onPositionChanged: {
                    if (pressed) {
                        cursorShape = Qt.CloseHandCursor;

                        if (chrome.window.maximized)
                            chrome.window.sendUnmaximized();
                    }
                }
                onDoubleClicked: {
                    if (chrome.window.maximized)
                        chrome.window.sendUnmaximized();
                    else
                        chrome.window.sendMaximized(shellSurfaceItem.output);
                }
            }

            Row {
                id: windowControls

                anchors {
                    right: parent.right
                    rightMargin: 8
                    verticalCenter: parent.verticalCenter
                }

                spacing: 12

                DecorationButton {
                    source: "window-minimize.svg"
                    color: foregroundColor
                    onClicked: chrome.window.minimized = true
                }

                DecorationButton {
                    source: chrome.window.maximized ? "window-restore.svg" : "window-maximize.svg"
                    color: foregroundColor
                    visible: chrome.window.maximizable
                    onClicked: chrome.window.maximized ? chrome.window.sendUnmaximized() : chrome.window.sendMaximized(shellSurfaceItem.output)
                }

                DecorationButton {
                    source: "window-close.svg"
                    color: foregroundColor
                    onClicked: chrome.window.sendClose()
                }
            }
        }
    }
}
