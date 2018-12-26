/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Liri.Shell 1.0 as LS

MouseArea {
    id: decoration

    readonly property real borderSize: {
        if (!chrome.decorated || shellSurface.maximized || shellSurface.fullscreen)
            return 0;
        return 4;
    }
    readonly property alias titleBarHeight: titleBar.height

    readonly property alias drag: moveArea.drag

    Material.theme: Material.Dark

    QtObject {
        id: d

        property int edges: 0
        property point initialPos
        property size initialSize
    }

    acceptedButtons: Qt.LeftButton

    hoverEnabled: enabled
    scrollGestureEnabled: false

    onPressed: {
        d.edges = 0;
        d.initialPos.x = mouse.x;
        d.initialPos.y = mouse.y;
        d.initialSize.width = shellSurfaceItem.width;
        d.initialSize.height = shellSurfaceItem.height;

        if (mouse.x <= borderSize)
            d.edges |= Qt.LeftEdge;
        else if (mouse.x >= this.width - borderSize)
            d.edges |= Qt.RightEdge;
        if (mouse.y <= borderSize)
            d.edges |= Qt.TopEdge;
        else if (mouse.y >= this.height - borderSize)
            d.edges |= Qt.BottomEdge;
    }
    onPositionChanged: {
        if (pressed) {
            var delta = Qt.point(mouse.x - d.initialPos.x, mouse.y - d.initialPos.y);
            shellSurface.interactiveResize(d.initialSize, delta, d.edges);
        }
    }

    Rectangle {
        id: titleBar

        x: borderSize
        y: borderSize

        implicitWidth: shellSurfaceItem.width
        implicitHeight: chrome.decorated ? 32 : 0

        color: Material.color(Material.Blue)

        visible: chrome.decorated

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

                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onPressed: {
                    if (mouse.button === Qt.LeftButton)
                        shellSurfaceItem.takeFocus();
                }
                onReleased: {
                    shellHelper.grabCursor(LS.ShellHelper.ArrowGrabCursor);

                    if (mouse.button === Qt.RightButton)
                        chrome.showWindowMenu(mouse.x, mouse.y);
                }
                onPositionChanged: {
                    if (pressed)
                        shellHelper.grabCursor(LS.ShellHelper.MoveGrabCursor);
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
                    onClicked: shellSurface.maximized ? chrome.unmaximize() : chrome.maximize(output)
                }

                DecorationButton {
                    source: "window-close.svg"
                    onClicked: shellSurface.close()
                }
            }
        }
    }
}
