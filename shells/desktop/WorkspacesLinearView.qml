/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0

WorkspacesView {
    id: root
    view: listView
    onWorkspaceAdded: {
        workspaces.model++;
    }
    onWorkspaceRemoved: {
        // Reparent all windows to the previous or next workspace
        var prevIndex = index == 0 ? 1 : index - 1;
        var workspace = workspaces.itemAt(index);
        var prevWorkspace = workspaces.itemAt(prevIndex);
        var i;
        for (i = 0; i < workspace.children.length; i++) {
            var window = workspace.children[i];
            window.parent = prevWorkspace;
        }

        // Remove item
        listView.currentIndex = prevIndex;
        workspaces.model--;
    }
    onWorkspaceSelected: {
        listView.currentIndex = index;
    }

    Flickable {
        property int currentIndex: -1
        property Workspace currentItem: workspaces.itemAt(currentIndex)
        property int count: workspaces.model

        id: listView
        anchors.fill: parent
        interactive: false
        contentX: currentIndex * root.width
        contentY: 0
        contentWidth: root.width
        contentHeight: root.height
        onCurrentIndexChanged: console.debug("Selected workspace", currentIndex)

        Behavior on contentX {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 250
            }
        }

        Item {
            width: root.width * workspaces.model
            height: root.height

            function selectWorkspace(item) {
                if (currentIndex !== item.workspaceIndex)
                    root.workspaceSelected(item.workspaceIndex);
            }

            Repeater {
                id: workspaces
                model: 0

                Workspace {
                    x: index * width
                    y: 0
                    width: listView.width
                    height: listView.height
                    workspaceIndex: index
                }
            }
        }
    }
}
