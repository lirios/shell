/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import GreenIsland 1.0
import Hawaii.Themes 1.0 as Themes

Item {
    property Item workspace: null

    id: root

    function run() {
        // Sanity check
        if (!workspace) {
            console.error("Running RevealDesktop without a workspace, cannot continue.");
            return;
        }

        // If there are no windows don't spare CPU time and exit
        var num = workspace.children.length;
        if (num === 0)
            return 0;

        // Divide screen in 4 zones
        var halfWidth = output.window.width * 0.5;
        var halfHeight = output.window.height * 0.5;
        var topLeft = Qt.rect(0, 0, halfWidth, halfHeight);
        var topRight = Qt.rect(topLeft.width, 0, halfWidth, halfHeight);
        var bottomLeft = Qt.rect(0, topLeft.height, halfWidth, halfHeight);
        var bottomRight = Qt.rect(bottomLeft.width, topLeft.height, halfWidth, halfHeight);
        var contains = function(r, x, y) {
            if (x <= r.x || x >= r.x + r.width)
                return false;
            if (y <= r.y || y >= r.y + r.height)
                return false;
            return true;
        };
        var margin = Themes.Units.dp(128);

        // Loop over windows
        var i, window;
        for (i = 0; i < num; i++) {
            // Find window
            window = workspace.children[i];

            // Skip certain windows
            if (!isWindowFine(window))
                continue;

            // Save original properties
            if (!window.savedProperties.saved) {
                window.savedProperties.x = window.x;
                window.savedProperties.y = window.y;
                window.savedProperties.saved = true;
            }

            // Move the window into the closest of the 4 zones
            if (contains(topLeft, window.x, window.y)) {
                window.x = topLeft.x - window.width + margin;
                window.y = topLeft.y - window.height + margin;
            } else if (contains(topRight, window.x, window.y)) {
                window.x = topRight.x + topRight.width - margin;
                window.y = topRight.y - window.height + margin;
            } else if (contains(bottomLeft, window.x, window.y)) {
                window.x = bottomLeft.x - window.width + margin;
                window.y = bottomLeft.y + bottomLeft.height - margin;
            } else if (contains(bottomRight, window.x, window.y)) {
                window.x = bottomRight.x + bottomRight.width - margin;
                window.y = bottomRight.y + bottomRight.height - margin;
            }
        }
    }

    function addWindow(window) {
        run();
    }

    function removeWindow(window) {
        run();
    }

    function end() {
        // Sanity check
        if (!workspace) {
            console.error("Running RevealDesktop without a workspace, cannot continue.");
            return;
        }

        // If there are no windows don't spare CPU time and exit
        var num = workspace.children.length;
        if (num === 0)
            return 0;

        // Loop over windows to restore saved properties
        var i, window;
        for (i = 0; i < num; i++) {
            // Find window
            window = workspace.children[i];

            // Skip certain windows
            if (!isWindowFine(window))
                continue;

            // Restore saved properties
            window.x = window.savedProperties.x;
            window.y = window.savedProperties.y;
            window.savedProperties.saved = false;
        }
    }

    function isWindowFine(window) {
        // Only windows from applications
        if (window.objectName !== "clientWindow")
            return false;

        // Skip windows from other outputs
        if (window.clientWindow.output !== _greenisland_output)
            return false;

        // Skip windows that are not toplevel
        if (window.clientWindow.type !== ClientWindow.TopLevel)
            return false;

        return true;
    }
}
