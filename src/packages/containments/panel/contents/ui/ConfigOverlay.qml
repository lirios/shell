/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

import QtQuick 2.1
import QtQuick.Layouts 1.0
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Shell 1.0
import Hawaii.Shell.Core 1.0
import "../code/LayoutManager.js" as LayoutManager

MouseArea {
    id: configurationArea
    z: 1000
    anchors.fill: root
    cursorShape: pressed ? Qt.DragMoveCursor : Qt.ArrowCursor
    hoverEnabled: true

    property Item currentElement

    property int lastX
    property int lastY

    onPositionChanged: {
        if (pressed) {
            if (root.Containment.formFactor === Types.Vertical) {
                currentElement.y += (mouse.y - lastY);
                handle.y = currentElement.y;
            } else {
                currentElement.x += (mouse.x - lastX);
                handle.x = currentElement.x;
            }

            lastX = mouse.x;
            lastY = mouse.y;

            var item = currentLayout.childAt(mouse.x, mouse.y);

            if (item && item !== placeHolder) {
                placeHolder.width = item.width;
                placeHolder.height = item.height;
                placeHolder.parent = configurationArea;
                var posInItem = mapToItem(item, mouse.x, mouse.y);

                if ((root.Containment.formFactor === Types.Vertical && posInItem.y < item.height/2) ||
                        (root.Containment.formFactor !== Types.Vertical && posInItem.x < item.width/2)) {
                    LayoutManager.insertBefore(item, placeHolder);
                } else {
                    LayoutManager.insertAfter(item, placeHolder);
                }
            }
        } else {
            var item = currentLayout.childAt(mouse.x, mouse.y);
            if (root.dragOverlay && item && item !== lastSpacer)
                root.dragOverlay.currentElement = item;
            else
                root.dragOverlay.currentElement = null;
        }
    }
    onCurrentElementChanged: {
        if (!root.dragOverlay.currentElement)
            return;

        handle.x = currentElement.x;
        handle.y = currentElement.y;
        handle.width = currentElement.width;
        handle.height = currentElement.height;
    }
    onPressed: {
        if (!root.dragOverlay.currentElement)
            return;

        lastX = mouse.x;
        lastY = mouse.y;
        placeHolder.width = currentElement.width;
        placeHolder.height = currentElement.height;
        LayoutManager.insertBefore(currentElement, placeHolder);
        currentElement.parent = root;
        currentElement.z = 900;
    }
    onReleased: {
        if (!root.dragOverlay.currentElement)
            return;

        LayoutManager.insertBefore(placeHolder, currentElement);
        placeHolder.parent = configurationArea;
        currentElement.z = 1;

        //LayoutManager.save();
    }

    Connections {
        target: currentElement
        onXChanged: handle.x = currentElement.x
        onYChanged: handle.y = currentElement.y
        onWidthChanged: handle.width = currentElement.width
        onHeightChanged: handle.height = currentElement.height
    }

    Item {
        id: placeHolder

        Layout.fillWidth: currentElement ? currentElement.Layout.fillWidth : false
        Layout.fillHeight: currentElement ? currentElement.Layout.fillHeight : false
    }

    Rectangle {
        id: handle
        color: "red"
        radius: 6
        antialiasing: true
        opacity: currentElement ? 0.5 : 0.0

        Behavior on x {
            enabled: !configurationArea.pressed
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
        Behavior on y {
            enabled: !configurationArea.pressed
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
        Behavior on width {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
        Behavior on height {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
        Behavior on opacity {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
    }
}
