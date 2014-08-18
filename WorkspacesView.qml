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

import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    readonly property Item currentWorkspace: __priv.currentIndex >= 0 ? get(__priv.currentIndex) : null
    readonly property alias currentIndex: __priv.currentIndex
    property Item view

    signal workspaceAdded(Item workspace)
    signal workspaceRemoved(int index);
    signal workspaceSelected(int index);

    id: root

    QtObject {
        id: __priv

        property int currentIndex: -1
    }

    ListModel {
        id: workspaceList
    }

    function get(index) {
        if ((index + 1) > workspaceList.count) {
            console.error("Workspace", index, "not found");
            return null;
        }

        return workspaceList.get(index).workspace;
    }

    function indexOf(workspace) {
        if (workspaceList.count == 0)
            return -1;

        var i;
        for (i = 0; i < workspaceList.count; i++) {
            if (workspaceList.get(i).workspace === workspace)
                return i;
        }

        return -1;
    }

    function add() {
        // Instantiate workspace component
        var component = Qt.createComponent("Workspace.qml");
        if (component.status === Component.Error) {
            // Bail out
            console.error("Cannot create a new workspace:",
                          component.errorString());
            return null;
        }

        // Create object and append to the model
        var workspace = component.createObject(root);
        workspaceList.append({"workspace": workspace});

        // Emit signal
        root.workspaceAdded(workspace);

        // Set current index if this is the first workspace
        if (__priv.currentIndex < 0) {
            __priv.currentIndex = 0;
            root.workspaceSelected(0);
        }

        console.debug("Added workspace", __priv.currentIndex);

        return workspace;
    }

    function remove(index) {
        // Find workspace
        var workspace = get(index);
        if (!workspace) {
            console.error("Workspace", index, "not found, cannot remove it");
            return;
        }

        // Find previous workspace
        var prevWorkspace = get(index - 1);
        if (!prevWorkspace) {
            console.error("Previous workspace not found, cannot remove workspace", index);
            return;
        }

        // Reparent all windows
        var i;
        for (i = 0; i < workspace.children; i++) {
            var window = workspace.children[i];
            window.parent = prevWorkspace;
        }

        // Transition to the previous workspace
        selectWorkspace(index - 1);

        // Remove workspace and destroy it
        workspaceList.remove(index);
        workspace.destroy();

        // Emit signal
        root.workspaceRemoved(index);
    }

    function select(index) {
        // Find workspace
        var workspace = get(index);
        if (!workspace) {
            console.error("Workspace", index, "not found, cannot switch to it");
            return;
        }

        // Emit signal
        root.workspaceSelected(index);

        // Switch to workspace
        __priv.currentIndex = index;
        console.debug("Switched to workspace", index);
    }

    function selectPrevious() {
        // Previous index (avoid overflow)
        var prevIndex = __priv.currentIndex - 1;
        if (prevIndex < 0)
            prevIndex = workspaceList.count - 1;

        // Select workspace
        select(prevIndex);
    }

    function selectNext() {
        // Next index (avoid overflow)
        var nextIndex = __priv.currentIndex + 1;
        if (nextIndex >= workspaceList.count)
            nextIndex = 0;

        // Select workspace
        select(nextIndex);
    }
}
