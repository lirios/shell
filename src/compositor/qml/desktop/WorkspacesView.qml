/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
import Fluid.Controls 1.0 as FluidControls

Row {
    id: swipeView

    readonly property int count: liriCompositor.settings.numWorkspaces
    property int currentWorkspaceNumber: 1
    property Item currentWorkspace: defaultWorkspace

    width: parent.width
    height: parent.height

    onCurrentWorkspaceNumberChanged: {
        console.debug("Selected workspace", currentWorkspaceNumber);
        swipeView.x = -width * (currentWorkspaceNumber - 1);
        if (currentWorkspaceNumber == 1)
            currentWorkspace = defaultWorkspace;
        else
            currentWorkspace = repeater.itemAt(currentWorkspaceNumber - 2);
    }

    Workspace {
        id: defaultWorkspace
    }

    Repeater {
        id: repeater

        model: swipeView.count - 1

        Workspace {}
    }

    Behavior on x {
        NumberAnimation { duration: FluidControls.Units.mediumDuration }
    }

    function select(num) {
        if (num < 1 || num > swipeView.count) {
            console.warn("Attempt to select an unknown workspace", num);
            return;
        }

        swipeView.currentWorkspaceNumber = num;

        if (currentWorkspace.state != "normal")
            currentWorkspace.stopEffect(currentWorkspace.state);
    }

    function selectPrevious() {
        // Previous index (avoid overflow)
        var prevNumber = swipeView.currentWorkspaceNumber - 1;
        if (prevNumber < 1)
            prevNumber = 1;

        // Select workspace
        select(prevNumber);

        // Show overlay
        osdServer.ShowText("navigation/arrow_back");
    }

    function selectNext() {
        // Next index (avoid overflow)
        var nextNumber = swipeView.currentWorkspaceNumber + 1;
        if (nextNumber > swipeView.count)
            nextNumber = swipeView.count;

        // Select workspace
        select(nextNumber);

        // Show overlay
        osdServer.ShowText("navigation/arrow_forward");
    }
}
