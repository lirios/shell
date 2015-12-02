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
import QtQuick.Controls 1.2

Flickable {
    readonly property alias currentIndex: d.currentIndex
    readonly property alias currentWorkspace: d.currentWorkspace
    readonly property alias count: workspaces.model

    id: root
    interactive: false
    contentX: currentIndex * width
    contentY: 0
    contentWidth: width
    contentHeight: height
    onCurrentIndexChanged: {
        console.debug("Selected workspace", currentIndex + 1);
        d.currentWorkspace = workspaces.itemAt(currentIndex);
    }

    Behavior on contentX {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: 250
        }
    }

    Item {
        width: parent.width * workspaces.model
        height: parent.height

        function selectWorkspace(item) {
            if (d.currentIndex !== item.workspaceIndex)
                root.select(item.workspaceIndex);
        }

        Repeater {
            id: workspaces
            model: 0

            Workspace {
                x: index * width
                y: 0
                width: root.width
                height: root.height
                workspaceIndex: index
            }
        }
    }

    QtObject {
        id: d

        property int currentIndex: -1
        property Workspace currentWorkspace: null

        function showOverlay(next) {
            // Show overlay
            var overlay = output.screenView.layers.overlays;
            overlay.iconName = next ? "go-next-symbolic" : "go-previous-symbolic";
            overlay.value = "";
            overlay.showProgress = false;
            overlay.timeout = 1000;
            if (!overlay.visible)
                overlay.show();
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
        d.currentIndex = prevIndex;
        workspaces.model--;
    }

    function select(num) {
        if (num < 1 || num > workspaces.count) {
            console.warn("Attempt to select unknown workspace", num);
            return;
        }

        d.currentIndex = num - 1;
    }

    function selectPrevious() {
        // Previous index (avoid overflow)
        var prevIndex = d.currentIndex - 1;
        if (prevIndex < 0)
            prevIndex = root.count - 1;
        if (prevIndex < 0)
            prevIndex = 0;

        // Select workspace
        select(prevIndex);

        // Show overlay
        d.showOverlay(false);
    }

    function selectNext() {
        // Next index (avoid overflow)
        var nextIndex = d.currentIndex + 1;
        if (nextIndex >= root.count)
            nextIndex = 0;

        // Select workspace
        select(nextIndex);

        // Show overlay
        d.showOverlay(true);
    }

    Component.onCompleted: {
        // Create all workspaces
        var i;
        for (i = 1; i <= hawaiiCompositor.settings.numWorkspaces; i++)
            add();
        select(1);
    }
}
