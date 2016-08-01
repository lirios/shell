/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Templates 2.0 as T
import Fluid.Controls 1.0

T.SwipeView {
    id: swipeView
    contentItem: ListView {
        model: swipeView.contentModel
        currentIndex: swipeView.currentIndex
        spacing: swipeView.spacing
        orientation: Qt.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        highlightRangeMode: ListView.StrictlyEnforceRange
        preferredHighlightBegin: 0
        preferredHighlightEnd: 0
        highlightMoveDuration: Units.mediumDelay
        interactive: false
    }
    onCurrentIndexChanged: console.debug("Selected workspace", currentIndex)

    QtObject {
        id: d

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

    Component {
        id: workspaceComponent

        Workspace {}
    }

    Workspace {}

    function add() {
        var incubator = workspaceComponent.incubateObject(swipeView, {});
        if (incubator.status !== Component.Ready) {
            incubator.onStatusChange = function(status) {
                if (status === Component.Ready)
                    swipeView.add(incubator.object);
            }
        } else {
            swipeView.add(incubator.object);
        }
    }

    function remove(index) {
        // Reparent all windows to the previous or next workspace
        var workspace = swipeView.itemAt(index);
        var prevIndex = index === 0 ? swipeView.count - 1 : index - 1;
        var prevWorkspace = swipeView.itemAt(prevIndex);
        var i;
        for (i = 0; i < workspace.children.length; i++) {
            var window = workspace.children[i];
            window.parent = prevWorkspace;
        }

        // Remove item
        swipeView.currentIndex = prevIndex;
        swipeView.removeItem(index);
    }

    function select(num) {
        if (num < 0 || num >= swipeView.count) {
            console.warn("Attempt to select unknown workspace", num);
            return;
        }

        swipeView.currentIndex = num;
    }

    function selectPrevious() {
        // Previous index (avoid overflow)
        var prevIndex = swipeView.currentIndex - 1;
        if (prevIndex < 0)
            prevIndex = swipeView.count - 1;
        if (prevIndex < 0)
            prevIndex = 0;

        // Select workspace
        select(prevIndex);

        // Show overlay
        d.showOverlay(false);
    }

    function selectNext() {
        // Next index (avoid overflow)
        var nextIndex = swipeView.currentIndex + 1;
        if (nextIndex >= swipeView.count)
            nextIndex = 0;

        // Select workspace
        select(nextIndex);

        // Show overlay
        d.showOverlay(true);
    }

    Component.onCompleted: {
        // Create all workspaces
        var i;
        for (i = swipeView.count; i < compositor.settings.numWorkspaces; i++)
            add();
        select(0);
    }
}
