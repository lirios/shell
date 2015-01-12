/****************************************************************************
 * This file is part of Green Island.
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
import QtCompositor 1.0

Item {
    property Item workspace: null

    id: root

    function run() {
        // Sanity check
        if (!workspace) {
            console.error("Running PresentWindowsGrid without a workspace, cannot continue.");
            return;
        }

        // If there are no windows don't spare CPU time and exit
        var num = workspace.children.length;
        if (num === 0)
            return 0;

        // Disable input in windows
        compositorRoot.disableInput();

        // Disable output zoom
        compositorRoot.screenView.zoomEnabled = false;

        // Calculate rows and columns
        var columns = Math.ceil(Math.sqrt(num));
        var rows = Math.ceil(num / columns);

        // Calculate cell size
        var cellWidth = compositorRoot.width / columns;
        var cellHeight = compositorRoot.height / rows;

        // Loop over windows
        var i, index = 0, ix = 0, iy = 0, lastDim = 1, window;
        for (i = 0; i < num; i++) {
            // Find window
            window = workspace.children[i];

            // Only windows from applications
            if (window.objectName !== "clientWindow")
                continue;

            // Skip windows from other outputs
            if (window.child.mainOutput !== _greenisland_output)
                continue;

            // Skip windows that are not toplevel
            if (window.child.surface.windowType !== WaylandQuickSurface.Toplevel)
                continue;

            // Save original properties
            if (!window.savedProperties.saved) {
                window.savedProperties.x = window.x;
                window.savedProperties.y = window.y;
                window.savedProperties.z = window.z;
                window.savedProperties.scale = window.scale;
                window.savedProperties.chrome = window.chrome;
                if (window.savedProperties.chrome)
                    window.savedProperties.chrome.visible = false;
                window.savedProperties.saved = true;
            }

            // Calculate grid
            if (index > 0) {
                var currentDim = Math.ceil(Math.sqrt(i + 1));
                if (currentDim == lastDim) {
                    if (iy < currentDim - 1) {
                        ++iy;

                        if (iy == currentDim - 1)
                            ix = 0;
                    } else {
                        ++ix;
                    }
                } else {
                    iy = 0;
                    ix = currentDim - 1;
                }
                lastDim = currentDim;
            }

            var cx = (ix + 0.5) * cellWidth;
            var cy = (iy + 0.5) * cellHeight;

            // Create a chrome
            if (!window.chrome) {
                var chromeComponent = Qt.createComponent("WindowChrome.qml");
                var chrome = chromeComponent.createObject(window);
                window.chrome = chrome;
                window.chrome.anchors.fill = window;
                window.chrome.clicked.connect(function(w) {
                    compositorRoot.enableInput();
                    w.savedProperties.bringToFront = true;
                    compositorRoot.endEffect("PresentWindowsGrid");
                });

                // Put the window behind its chrome
                window.z = 0;
                window.chrome.z = 1;
            }

            // Apply new properties
            window.x = cx - window.width / 2;
            window.y = cy - window.height / 2;
            window.z = 1;
            window.scale = 0.98 * Math.min(cellWidth / window.width, cellHeight / window.height);

            index++;
        }
    }

    function addWindow(window) {
        run();
    }

    function removeWindow(window) {
        run();
    }

    function end() {
        // Enable input in windows
        compositorRoot.enableInput();

        // Enable output zoom again
        compositorRoot.screenView.zoomEnabled = true;

        // Sanity check
        if (!workspace) {
            console.error("Running PresentWindowsGrid without a workspace, cannot continue.");
            return;
        }

        // If there are no windows don't spare CPU time and exit
        var num = workspace.children.length;
        if (num === 0)
            return 0;

        // Window that was selected with a click before
        var selectedWindow = null;

        // Loop over windows to restore saved properties
        var i, window;
        for (i = 0; i < num; i++) {
            // Find window
            window = workspace.children[i];
            if (window.objectName !== "clientWindow")
                continue;

            // Remove chrome
            if (window.chrome) {
                window.chrome.destroy();
                window.chrome = null;
            }

            // Restore saved properties
            window.x = window.savedProperties.x;
            window.y = window.savedProperties.y;
            window.z = window.savedProperties.z;
            window.scale = window.savedProperties.scale;
            window.chrome = window.savedProperties.chrome;
            if (window.chrome)
                window.chrome.visible = true;
            window.savedProperties.saved = false;

            // Bring window to front
            if (window.savedProperties.bringToFront) {
                selectedWindow = window;
                window.savedProperties.bringToFront = false;
            }
        }

        if (selectedWindow)
            compositorRoot.moveFront(selectedWindow);
    }
}
