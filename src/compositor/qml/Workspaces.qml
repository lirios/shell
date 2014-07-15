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

Item {
    property alias model: container.model
    property var currentWorkspace: workspacesModel.children[currentWorkspaceIndex]
    property alias currentWorkspaceIndex: container.currentIndex

    id: workspaces

    PathView {
        id: container
        anchors.fill: parent
        highlightRangeMode: PathView.StrictlyEnforceRange
        preferredHighlightBegin: 0.0
        preferredHighlightEnd: 1.0
        flickDeceleration: 5000
        highlightMoveDuration: 250
        interactive: false
        model: VisualItemModel {
            id: workspacesModel

            Workspace {}
            Workspace {}
            Workspace {}
            Workspace {}
        }
        currentIndex: -1
        snapMode: PathView.SnapOneItem
        clip: true
        path: Path {
            startX: - width * model.count / 2 + width / 2
            startY: height / 2
            PathLine {
                x: width * model.count / 2 + width / 2
                y: height / 2
            }
        }
    }

    function addWorkspace() {
        var index = workspacesModel.count;

        // Create workspace item
        //var component = Qt.createComponent("Workspace.qml");
        //var item = component.createObject(container);

        // First workspace created become the current one
        console.log("*********** prima", currentWorkspaceIndex);
        if (currentWorkspaceIndex < 0)
            currentWorkspaceIndex = 0;
        console.log("*********** dopo", currentWorkspaceIndex);
    }

    function removeWorkspace(index) {
        // Find workspace
        var workspace = workspacesModel.get(index);
        if (typeof(workspace) == "undefined") {
            console.warn("Workspace " + index + " not found!");
            return;
        }

        // Find previous workspace
        var prevWorkspace = workspacesModel.get(index - 1);
        if (typeof(prevWorkspace) == "undefined") {
            console.warn("Workspace " + (index - 1) + " not found!");
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
        workspacesModel.remove(index - 1);
        workspace.destroy();
    }

    function selectWorkspace(index) {
        // Set next workspace position
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
