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
import GreenIsland 1.0
import Hawaii.Themes 1.0 as Themes

Item {
    property Item workspace: null

    id: root

    QtObject {
        id: __priv

        property bool debug: false
        property var selectedWindow: null
        readonly property real hmargin: Themes.Units.dp(48)
        readonly property real vmargin: Themes.Units.dp(48)
    }

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

        // Save currently active window, it will be activated again
        // if the effect is stopped without clicking on a window
        __priv.selectedWindow = compositorRoot.activeWindow

        // Layout
        gridLayout(num);
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

    function gridLayout(num) {
        // Calculate rows and columns
        var columns = Math.ceil(Math.sqrt(num));
        var rows = Math.ceil(num / columns);
        if (__priv.debug) console.log("rows:", rows, "columns:", columns);

        // Calculate cell size
        var cellWidth = _greenisland_output.availableGeometry.width / columns;
        var cellHeight = _greenisland_output.availableGeometry.height / rows;
        if (__priv.debug) console.log("cellWidth:", cellWidth, "cellHeight:", cellHeight);

        // Loop over windows
        var i, window, scale;
        var currentDim = 0, lastDim = 1, row = 0, column = 0;
        for (i = 0; i < num; i++) {
            // Find window
            window = workspace.children[i];
            if (!isWindowFine(window))
                continue;

            // Save original properties
            if (!window.savedProperties.saved) {
                window.savedProperties.x = window.x;
                window.savedProperties.y = window.y;
                window.savedProperties.width = window.width;
                window.savedProperties.height = window.height;
                window.savedProperties.scale = window.scale;
                window.savedProperties.chrome = window.chrome;
                if (window.savedProperties.chrome)
                    window.savedProperties.chrome.visible = false;
                window.savedProperties.saved = true;
            }

            // Calculate grid
            if (i > 0) {
                currentDim = Math.ceil(Math.sqrt(i + 1));
                if (currentDim == lastDim) {
                    if (row < currentDim - 1) {
                        ++row;

                        if (row == currentDim - 1)
                            column = 0;
                    } else {
                        ++column;
                    }
                } else {
                    row = 0;
                    column = currentDim - 1;
                }
                lastDim = currentDim;
            }
            if (__priv.debug) console.log("row:", row, "column:", column);

            // Move window
            window.x = (cellWidth * column) + __priv.hmargin;
            window.y = (cellHeight * row) + __priv.vmargin;
            //if (__priv.debug) console.log("x:", window.x, "y:", window.y);

            // Resize window
            scale = Math.min((cellWidth - __priv.hmargin) / window.child.surface.size.width,
                             (cellHeight - __priv.vmargin) / window.child.surface.size.height) * 0.98;
            window.width = window.child.surface.size.width * scale;
            window.height = window.child.surface.size.height * scale;
            //if (__priv.debug) console.log("window:", window.clientWindow.id, "scale:", scale);

            // Create a chrome
            if (!window.chrome) {
                var chromeComponent = Qt.createComponent("WindowChrome.qml");
                window.chrome = chromeComponent.createObject(window, {"window": window});
                window.chrome.clicked.connect(function(w) {
                    w.savedProperties.bringToFront = true;
                    compositorRoot.endEffect("PresentWindowsGrid");
                });
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
            window.width = window.savedProperties.width;
            window.height = window.savedProperties.height;
            window.scale = window.savedProperties.scale;
            window.chrome = window.savedProperties.chrome;
            if (window.chrome)
                window.chrome.visible = true;
            window.savedProperties.saved = false;

            // Bring window to front
            if (window.savedProperties.bringToFront) {
                __priv.selectedWindow = window;
                window.savedProperties.bringToFront = false;
            }
        }

        // Activate selected window
        if (__priv.selectedWindow && __priv.selectedWindow.clientWindow)
            __priv.selectedWindow.clientWindow.activate();
    }
}
