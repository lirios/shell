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
import GreenIsland.Core 1.0

Item {
    property int currentWorkspaceIndex: 0

    id: workspaces

    ListModel {
        id: workspacesModel
    }

    StackView {
        id: stackView
        anchors.fill: parent
    }

    function getObject(index) {
        var object = workspacesModel.get(index);
        if (typeof(object) == "undefined") {
            console.error("Workspace", index, "not found");
            return null;
        }

        return object;
    }

    function get(index) {
        var object = getObject(index);
        if (object)
            return object.workspace;
        return null;
    }

    function indexOf(workspace) {
        var i;
        for (i = 0; workspacesModel.count; i++) {
            if (workspacesModel.get(i).workspace === workspace)
                return i;
        }

        return -1;
    }

    function addWorkspace() {
        // Instantiate workspace component
        var component = Qt.createComponent("Workspace.qml");
        if (component.status === Component.Error) {
            // Bail out
            console.error("Cannot create a new workspace:",
                          component.errorString());
            return null;
        }

        // Create object and append to the model
        var workspace = component.createObject(workspaces);
        workspacesModel.append({"workspace": workspace});

        return workspace;
    }

    function removeWorkspace(index) {
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

        // Remove workspace
        workspacesModel.remove(index);
        stackView.pop({"immediate": true});
        workspace.destroy();
    }

    function selectWorkspace(index) {
        // Find workspace
        var workspace = get(index);
        if (!workspace) {
            console.error("Workspace", index, "not found, cannot switch to it");
            return;
        }

        // Switch to workspace
        stackView.push({"item": workspace});
        currentWorkspaceIndex = index;
        console.debug("Switched to workspace", index);
    }

    function selectNextWorkspace() {
        // Next index (avoid overflow)
        var nextIndex = currentWorkspaceIndex + 1;
        if (nextIndex >= workspacesModel.count)
            nextIndex = 0;

        // Select workspace
        selectWorkspace(nextIndex);
    }
}
