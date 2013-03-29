/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import FluidCore 1.0
import FluidUi 1.0
import "PanelMenuManager.js" as PanelMenuManager

Item {
    id: menuItem

    default property alias items: contentsItem.children
    property alias text: label.text
    property alias font: label.font
    property bool separator: false

    property real padding: 4

    signal clicked

    implicitWidth: Math.max(label.paintedWidth + contentsItem.width, 150) + (padding * 3)
    implicitHeight: separator ? internal.separatorItem.height + (padding * 2) : Math.max(Math.max(label.paintedHeight, contentsItem.height), 30)
    width: Math.max(implicitWidth, parent.width)
    enabled: !separator

    Keys.onReturnPressed: {
        PanelMenuManager.currentIndicator.selected = false;
        PanelMenuManager.triggered = false;
        PanelMenuManager.currentIndicator = null;
        menuItem.clicked();
    }

    onSeparatorChanged: {
        if (separator)
            internal.separatorItem = separatorComponent.createObject(menuItem)
        else
            internal.separatorItem.destroy();
    }

    QtObject {
        id: internal

        property Item separatorItem
    }

    Component {
        id: separatorComponent

        FrameSvgItem {
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            imagePath: "widgets/menuitem"
            prefix: "separator"
            height: margins.top + margins.bottom
        }
    }

    FrameSvgItem {
        id: container
        anchors.fill: parent
        imagePath: "widgets/menuitem"
        prefix: ""
        opacity: separator ? 0.0 : prefix == "" ? 0.0 : 1.0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    Item {
        id: item

        anchors {
            fill: container
            leftMargin: container.margins.left + padding
            topMargin: container.margins.top
            rightMargin: container.margins.right + padding
            bottomMargin: container.margins.bottom
        }

        Label {
            id: label
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            visible: !separator
        }

        Row {
            id: contentsItem
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            visible: children.length > 0
            onChildrenChanged: {
                // Trigger the clicked() signal for child items too
                for (var i = 0; i < children.length; ++i) {
                    if (children[i].clicked != undefined)
                        children[i].clicked.connect(menuItem.clicked)
                }
            }
        }
    }

    MouseArea {
        anchors.fill: item
        hoverEnabled: true
        preventStealing: true
        onEntered: {
            label.color = theme.highlightedTextColor;
            container.prefix = "selected";
        }
        onExited: {
            label.color = theme.windowTextColor;
            container.prefix = "";
        }
        onClicked: {
            PanelMenuManager.currentIndicator.selected = false;
            PanelMenuManager.triggered = false;
            PanelMenuManager.currentIndicator = null;
            menuItem.clicked();
        }
    }
}
