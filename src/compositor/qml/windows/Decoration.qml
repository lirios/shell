/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects

Control {
    id: decoration

    readonly property real marginSize: 0
    readonly property real titleBarHeight: shellSurface.fullscreen ? 0 : 32 - titleBar.radius
    readonly property bool dragging: moveArea.drag.active
    readonly property bool hasDropShadow: !shellSurface.maximized && !shellSurface.fullscreen

    property Item dragTarget: shellSurfaceItem.moveItem

    Material.theme: Material.Dark

    // FIXME: Transparent backgrounds will be opaque due to shadows
    layer.enabled: hasDropShadow
    layer.effect: FluidEffects.Elevation {
        elevation: shellSurfaceItem.focus ? 24 : 8
    }

    padding: shellSurface.maximized || shellSurface.fullscreen ? 0 : (shellSurface.windowType === Qt.Popup ? 1 : 4)

    visible: shellSurface.decorated && !shellSurface.fullscreen

    MouseArea {
        id: resizeArea

        property int pressX
        property int pressY
        property int startW
        property int startH
        property bool pressed: false

        anchors.fill: parent
        hoverEnabled: true

        //bitfield: top, left, bottom, right
        property int edges
        onPressed: {
            pressed = true;
            edges = 0;
            pressX = mouse.x;
            pressY = mouse.y;
            startW = chrome.width;
            startH = chrome.height;
            if (mouse.y > chrome.height - titleBarHeight)
                edges |= 4; //bottom edge
            if (mouse.x > chrome.width - titleBarHeight)
                edges |= 8; //right edge
        }
        onReleased: pressed = false
        onMouseXChanged: {
            if (pressed) {
                var w = startW;
                var h = startH;
                if (edges & 8)
                    w += mouse.x - pressX;
                if (edges & 4)
                    h += mouse.y - pressY;
                shellSurface.requestSize(w, h);
            }
        }
    }

    Rectangle {
        id: titleBar

        anchors {
            left: parent.left
            top: parent.top
        }
        width: shellSurfaceItem.width
        height: titleBarHeight + radius
        radius: 3
        color: Material.color(Material.Blue)

        Item {
            anchors.fill: parent
            anchors.bottomMargin: parent.radius
            opacity: shellSurface.activated ? 1.0 : 0.5

            FluidControls.Icon {
                id: icon

                anchors {
                    left: parent.left
                    leftMargin: 8
                    verticalCenter: parent.verticalCenter
                }

                name: shellSurface.iconName
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
                color: Material.primaryTextColor
                text: shellSurface.title
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

                drag.target: dragTarget
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onPressed: {
                    if (mouse.button === Qt.LeftButton)
                        shellSurfaceItem.takeFocus();
                }
                onClicked: {
                    if (mouse.button === Qt.LeftButton)
                        shellSurfaceItem.takeFocus();
                    else if (mouse.button === Qt.RightButton)
                        chrome.showWindowMenu(mouse.x, mouse.y);
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
                    onClicked: shellSurface.minimized = true
                }

                DecorationButton {
                    source: shellSurface.maximized ? "window-restore.svg" : "window-maximize.svg"
                    onClicked: shellSurface.maximized ? shellSurface.unmaximize() : shellSurface.maximize(output)
                }

                DecorationButton {
                    source: "window-close.svg"
                    onClicked: shellSurface.close()
                }
            }
        }
    }
}
