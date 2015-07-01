/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
import QtQuick.Controls 1.2

Item {
    readonly property Item currentWorkspace: listView.currentItem
    readonly property int currentIndex: listView.currentIndex
    property alias view: listView

    id: root

    QtObject {
        id: __priv

        function showOverlay(next) {
            // Show overlay
            var overlay = compositorRoot.screenView.layers.overlays;
            overlay.iconName = next ? "go-next-symbolic" : "go-previous-symbolic";
            overlay.value = "";
            overlay.showProgress = false;
            overlay.timeout = 1000;
            if (!overlay.visible)
                overlay.show();
        }
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
                    root.select(item.workspaceIndex);
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

    function add() {
        workspaces.model++;
    }

    function remove(index) {
        // Reparent all windows to the previous or next workspace
        var prevIndex = index === 0 ? 1 : index - 1;
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

    function select(index) {
        listView.currentIndex = index;
    }

    function selectPrevious() {
        // Previous index (avoid overflow)
        var prevIndex = view.currentIndex - 1;
        if (prevIndex < 0)
            prevIndex = view.count - 1;
        if (prevIndex < 0)
            prevIndex = 0;

        // Select workspace
        select(prevIndex);

        // Show overlay
        __priv.showOverlay(false);
    }

    function selectNext() {
        // Next index (avoid overflow)
        var nextIndex = view.currentIndex + 1;
        if (nextIndex >= view.count)
            nextIndex = 0;

        // Select workspace
        select(nextIndex);

        // Show overlay
        __priv.showOverlay(true);
    }
}
