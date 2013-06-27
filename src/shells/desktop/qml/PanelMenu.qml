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

import QtQuick 2.1
import QtQuick.Controls 1.0
import DesktopShell 0.1

ShellWindow {
    id: panelMenu
    color: "transparent"
    width: menu.width
    height: menu.height
    visible: false

    default property alias content: menuContents.children

    Rectangle {
        id: menuContainer
        width: menu.width
        height: menu.height
        radius: 6
        border.color: "#999"
        color: "white"
        state: "hidden"
        enabled: panelMenu.visible

        Item {
            id: contentItem
            width: Math.max(menuContents.width, theme.defaultFont.mSize.width * 12)
            height: Math.min(menuContents.height, theme.defaultFont.mSize.height * 25)

            ScrollView {
                Flickable {
                    id: flickable
                    anchors.fill: parent

                    Column {
                        id: menuContents
                        spacing: 4
                        onChildrenChanged: {
                            // Close the popup menu when a menu item is clicked
                            connectClickedSignal();

                            // Resize the popup menu accordingly to its items
                            resizePopupMenu();
                        }
                        onChildrenRectChanged: resizePopupMenu()

                        function connectClickedSignal() {
                            for (var i = 0; i < children.length; ++i) {
                                if (children[i].clicked != undefined)
                                    children[i].clicked.connect(panelMenu.close);
                            }
                        }

                        function resizePopupMenu() {
                            panelMenu.width = childrenRect.width;
                            panelMenu.height = childrenRect.height;
                        }
                    }
                }
            }
        }

        states: [
            State {
                name: "visible"
                when: status == DialogStatus.Opening || status == DialogStatus.Open
                PropertyChanges {
                    target: menuContainer
                    //y: panelMenu.parent.parent.height + panelMenu.parent.y
                    opacity: 1.0
                }
            },
            State {
                name: "hidden"
                when: status == DialogStatus.Closing || status == DialogStatus.Closed
                PropertyChanges {
                    target: menuContainer
                    //y: 0.0
                    opacity: 0.0
                }
            }
        ]

        transitions: [
            Transition {
                from: "visible"
                to: "hidden"

                SequentialAnimation {
                    ParallelAnimation {
                        NumberAnimation {
                            target: menuContainer
                            property: "y"
                            duration: 50
                            easing.type: Easing.Linear
                        }
                        NumberAnimation {
                            target: menuContainer
                            property: "opacity"
                            duration: 200
                            easing.type: Easing.Linear
                        }
                    }
                    PropertyAction { target: panelMenu; property: "visible"; value: false }
                }
            },
            Transition {
                from: "hidden"
                to: "visible"

                SequentialAnimation {
                    ParallelAnimation {
                        NumberAnimation {
                            target: menuContainer
                            property: "y"
                            duration: 100
                            easing.type: Easing.Linear
                        }
                        NumberAnimation {
                            target: menuContainer
                            property: "opacity"
                            duration: 250
                            easing.type: Easing.Linear
                        }
                    }
                    PropertyAction { target: panelMenu; property: "visible"; value: true }
                }
            }
        ]
    }

    function open() {
        visible = true;
    }

    function close() {
        visible = false;
    }
}
