/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Shell 1.0 as Shell
//import Hawaii.Shell.Core 1.0
import Hawaii.Shell.Styles 1.0

Item {
    id: panelView

    property Item currentLayout: view.formFactor === Shell.Types.Horizontal ? row : column

    StyledItem {
        id: styledItem
        anchors.fill: parent
        style: Qt.createComponent(StyleSettings.path + "/PanelStyle.qml", panelView)

        Item {
            id: handle
            width: view.formFactor === Shell.Types.Horizontal ? parent.height : parent.width
            height: width
            visible: false
            states: [
                State {
                    name: "horizontal"
                    when: view.formFactor === Shell.Types.Horizontal

                    AnchorChanges {
                        target: handle
                        anchors.top: parent.top
                        anchors.right: parent.right
                    }
                },
                State {
                    name: "vertical"
                    when: view.formFactor === Shell.Types.Vertical

                    AnchorChanges {
                        target: handle
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                    }
                    PropertyChanges {
                        target: handle
                        anchors.leftMargin: (parent.width / 2) - (handle.width / 2)
                    }
                }
            ]

            FluidUi.Icon {
                anchors.fill: parent
                iconName: "preferences-system-windows"
            }

            Behavior on width {
                PropertyAnimation {}
            }

            Behavior on height {
                PropertyAnimation {}
            }
        }

        RowLayout {
            id: row
            states: [
                State {
                    name: "with-handle"
                    when: handle.visible

                    AnchorChanges {
                        target: row
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.right: handle.left
                        anchors.bottom: parent.bottom
                    }
                },
                State {
                    name: "without-handle"
                    when: !handle.visible

                    PropertyChanges {
                        target: row
                        anchors.fill: parent
                    }
                }
            ]
        }

        ColumnLayout {
            id: column
            states: [
                State {
                    name: "with-handle"
                    when: handle.visible

                    AnchorChanges {
                        target: row
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.bottom: handle.top
                    }
                },
                State {
                    name: "without-handle"
                    when: !handle.visible

                    PropertyChanges {
                        target: row
                        anchors.fill: parent
                    }
                }
            ]
        }

        Item {
            id: spacer
            //parent: currentLayout

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Component {
            id: elementComponent

            Item {
                id: elementContainer
                visible: false

                property Item element

                Layout.minimumWidth: {
                    if (view.formFactor !== Shell.Types.Vertical)
                        return element && element.minimumWidth > 0 ? element.minimumWidth : panelView.height;
                    return panelView.width;
                }
                Layout.minimumHeight: {
                    if (view.formFactor === Shell.Types.Vertical)
                        return element && element.minimumHeight > 0 ? element.minimumHeight : panelView.width;
                    return panelView.height;
                }

                Layout.preferredWidth: {
                    if (view.formFactor !== Shell.Types.Vertical)
                        return element && element.implicitWidth > 0 ? element.implicitWidth : panelView.height;
                    return panelView.width;
                }
                Layout.preferredHeight: {
                    if (view.formFactor === Shell.Types.Vertical)
                        return element && element.implicitHeight > 0 ? element.implicitHeight : panelView.width;
                    return panelView.height;
                }

                Layout.maximumWidth: {
                    if (view.formFactor !== Shell.Types.Vertical)
                        return element && element.maximumWidth > 0 ? element.maximumWidth : (Layout.fillWidth ? -1 : panelView.height);
                    return Layout.fillHeight ? -1 : panelView.width;
                }
                Layout.maximumHeight: {
                    if (view.formFactor === Shell.Types.Vertical)
                        return element && element.maximumHeight > 0 ? element.maximumHeight : (Layout.fillHeight ? -1 : panelView.width);
                    return Layout.fillWidth ? -1 : panelView.height;
                }

                Layout.fillWidth: element && element.fillWidth
                Layout.fillHeight: element && element.fillHeight

                BusyIndicator {
                    anchors.centerIn: parent
                    //visible: element && element.busy
                    visible: false
                    running: visible
                    z: 1000
                }
            }
        }
    }

    Connections {
        target: view
        onElementAdded: {
            addElement(element);
        }
    }

    function addElement(element) {
        var container = elementComponent.createObject(panelView);
        element.parent = container;
        container.element = element;
        element.anchors.fill = container;
        element.visible = true;
        container.visible = true;

        if (spacer.parent == currentLayout)
            insertBefore(spacer, container);
        else
            container.parent = currentLayout;
    }

    /*
     * This function was adapted from Plasma's panel containment.
     * Copyright (C) 2013 Marco Martin <mart@kde.org>
     */
    function insertBefore(item1, item2) {
        if (item1 === item2)
            return;

        var removed = new Array();
        var child;

        var i;
        for (i = currentLayout.children.length - 1; i >= 0; --i) {
            child = currentLayout.children[i];
            removed.push(child);
            child.parent = panelView;

            if (child === item1)
                break;
        }

        item2.parent = currentLayout;

        for (var j = removed.length - 1; j >= 0; --j)
            removed[j].parent = currentLayout;
        return i;
    }

    /*
     * This function was adapted from Plasma's panel containment.
     * Copyright (C) 2013 Marco Martin <mart@kde.org>
     */
    function insertAfter(item1, item2) {
        if (item1 === item2)
            return;

        var removed = new Array();
        var child;

        var i;
        for (i = currentLayout.children.length - 1; i >= 0; --i) {
            child = currentLayout.children[i];
            if (child === item1)
                break;

            removed.push(child);
            child.parent = root;
        }

        item2.parent = currentLayout;

        for (var j = removed.length - 1; j >= 0; --j)
            removed[j].parent = currentLayout;
        return i;
    }

    /*
     * This function was adapted from Plasma's panel containment.
     * Copyright (C) 2013 Marco Martin <mart@kde.org>
     */
    function insertAtIndex(item, position) {
        var removedItems = new Array();

        var i;
        for (i = position; i < v.children.length; ++i) {
            var child = currentLayout.children[position];
            child.parent = root;
            removedItems.push(child);
        }

        item.parent = currentLayout;
        for (var i in removedItems)
            removedItems[i].parent = currentLayout;
    }
}
