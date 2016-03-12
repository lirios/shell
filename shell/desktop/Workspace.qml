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
import Fluid.Ui 1.0 as FluidUi

Item {
    id: workspace
    state: "normal"
    states: [
        State {
            name: "normal"
        },
        State {
            name: "present"
        },
        State {
            name: "reveal"
        }
    ]
    transitions: [
        Transition {
            from: "normal"
            to: "present"
            ScriptAction { script: present() }
        },
        Transition {
            from: "present"
            to: "normal"
            ScriptAction { script: presentRestore() }
        },
        Transition {
            from: "normal"
            to: "reveal"
            ScriptAction { script: reveal() }
        },
        Transition {
            from: "reveal"
            to: "normal"
            ScriptAction { script: revealRestore() }
        }
    ]
    onChildrenChanged: {
        switch (state) {
        case "present":
            present();
            break;
        case "reveal":
            reveal();
            break;
        default:
            break;
        }
    }

    Component {
        id: chromeComponent

        PresentWindowsChrome {}
    }

    ListModel {
        id: viewsModel
    }

    function windowsList() {
        var windows = [];
        var i, window;
        for (i = 0; i < windowManager.windows.length; i++) {
            window = windowManager.windows[i];
            if (window.designedOutput === output)
                windows.push(window);
        }
        return windows;
    }

    function present() {
        console.time("present " + output.model);

        // Parameters
        var windows = windowsList();
        var hmargin = output.availableGeometry.width * 0.1;
        var vmargin = output.availableGeometry.height * 0.1;
        var padding = FluidUi.Units.gu(2);

        var gridSize = Math.floor(Math.sqrt(windows.length));
        if (Math.pow(gridSize, 2) !== windows.length)
            gridSize++;
        var lastRowRemoved = Math.pow(gridSize, 2) - windows.length;

        // Calculate cell size
        var cellWidth = output.availableGeometry.width - (hmargin * 2);
        cellWidth -= padding * (gridSize - 1);
        cellWidth /= gridSize;
        var cellHeight = output.availableGeometry.height - (vmargin * 2);
        cellHeight -= padding * (gridSize - 1);
        cellHeight /= gridSize;

        // Chrome size can't exceed output available geometry
        var chromeSize = cellWidth < cellHeight ? cellWidth : cellHeight;
        if (chromeSize > output.availableGeometry.width / 2)
            chromeSize = output.availableGeometry.width / 2;
        if (chromeSize > output.availableGeometry.height / 2)
            chromeSize = output.availableGeometry.height / 2;

        // Padding between cells
        var cellPadding = chromeSize + padding;

        // Increase horizontal margin to center chromes
        var maxColumns = Math.ceil(Math.sqrt(windows.length));
        var maxWidth = maxColumns * cellPadding;
        hmargin = (output.availableGeometry.width - maxWidth) / 2;

        // Loop over windows
        console.time("present loop " + output.model);
        var i, j, window;
        for (i = 0; i < windows.length; i++) {
            // Get a hold of the window
            window = windows[i];

            // Determine row and column
            var row = Math.floor(i / gridSize);
            var column = Math.floor(i % gridSize);

            // Output space starting coordinates
            var x = window.x - window.designedOutput.geometry.x;
            var y = window.y - window.designedOutput.geometry.y;

            // Determine new coordinates
            var x2 = hmargin + cellPadding * column;
            var y2 = vmargin + cellPadding * row;

            // Calculate window scale
            if (row == gridSize - 1)
                x2 += (chromeSize + padding) * lastRowRemoved / 2;
            var scale = 1;
            if (window.surface.size.width > window.surface.size.height)
                scale = chromeSize / window.surface.size.width;
            else
                scale = chromeSize / window.surface.size.height;

            // Hide all views
            for (j = 0; j < window.views.length; j++)
                window.views[j].visible = false;

            // Create chrome or move an existing one
            var chrome = null;
            for (j = 0; j < viewsModel.count; j++) {
                var thisChrome = viewsModel.get(j).chrome;
                if (thisChrome.window === window) {
                    chrome = thisChrome;
                    break;
                }
            }
            if (chrome) {
                chrome.move(x2, y2);
            } else {
                chrome = chromeComponent.createObject(workspace, {"window": window, "x": x, "y": y});
                if (chrome) {
                    for (j = 0; j < window.views.length; j++) {
                        if (window.views[j].designedOutput === output) {
                            chrome.z = window.views[j].z;
                            break;
                        }
                    }
                    chrome.move(x2, y2);
                    chrome.resize(scale);
                    chrome.activated.connect(function(window) {
                        state = "normal";
                        window.active = true;
                    });
                    chrome.closeRequested.connect(function(window) {
                        state = "normal";
                        window.close();
                    });

                    viewsModel.append({"chrome": chrome});
                } else {
                    console.error("Failed to create chrome:", chromeComponent.errorString());
                }
            }
        }
        console.timeEnd("present loop " + output.model);

        console.timeEnd("present " + output.model);
    }

    function presentRestore() {
        // Destroy all chromes and show the views
        var i, chrome, x, y;
        while (viewsModel.count > 0) {
            chrome = viewsModel.get(0).chrome;
            x = chrome.window.x - chrome.window.designedOutput.geometry.x;
            y = chrome.window.y - chrome.window.designedOutput.geometry.y;
            chrome.move(x, y);
            chrome.width = chrome.window.surface.size.width;
            chrome.height = chrome.window.surface.size.height;
            chrome.triggerAutodestruction();
            viewsModel.remove(0);
        }
    }

    function reveal() {
        console.time("reveal " + output.model);

        var windows = windowsList();
        var margin = FluidUi.Units.dp(96);

        // Divide screen in 4 zones
        var halfWidth = output.availableGeometry.width * 0.5;
        var halfHeight = output.availableGeometry.height * 0.5;
        var topLeft = Qt.rect(output.position.x, output.position.y, halfWidth, halfHeight);
        var topRight = Qt.rect(output.position.x + topLeft.width, output.position.y, halfWidth, halfHeight);
        var bottomLeft = Qt.rect(output.position.x, output.position.y + topLeft.height, halfWidth, halfHeight);
        var bottomRight = Qt.rect(output.position.x + bottomLeft.width, output.position.y + topLeft.height, halfWidth, halfHeight);
        var contains = function(r, x, y) {
            if (x <= r.x || x >= r.x + r.width)
                return false;
            if (y <= r.y || y >= r.y + r.height)
                return false;
            return true;
        };

        // Loop over windows
        console.time("reveal loop " + output.model);
        var i, j, window, view, x, y;
        for (i = 0; i < windows.length; i++) {
            // Get a hold of the window
            window = windows[i];

            // Determine global coordinates to the closest of the 4 zones
            if (contains(topRight, window.x, window.y)) {
                x = topRight.x + topRight.width - margin;
                y = topRight.y - window.surface.size.height + margin;
            } else if (contains(bottomLeft, window.x, window.y)) {
                x = bottomLeft.x - window.surface.size.width + margin;
                y = bottomLeft.y + bottomLeft.height - margin;
            } else if (contains(bottomRight, window.x, window.y)) {
                x = bottomRight.x + bottomRight.width - margin;
                y = bottomRight.y + bottomRight.height - margin;
            } else {
                x = topLeft.x - window.surface.size.width + margin;
                y = topLeft.y - window.surface.size.height + margin;
            }

            for (j = 0; j < window.views.length; j++) {
                // Get a hold of the view
                view = window.views[j];

                // Enable animations
                view.animationsEnabled = true;

                // Save original properties
                if (!view.savedProperties.saved) {
                    view.savedProperties.x = view.x;
                    view.savedProperties.y = view.y;
                    view.savedProperties.saved = true;
                }

                // Move this view in output coordinates space
                view.x = x - view.output.position.x;
                view.y = y - view.output.position.y;

                // Hide on other outputs
                if (view.output !== output)
                    view.visible = false;
            }
        }
        console.timeEnd("reveal loop " + output.model);

        console.timeEnd("reveal " + output.model);
    }

    function revealRestore() {
        var windows = windowsList();
        var i, j, window, view;
        for (i = 0; i < windows.length; i++) {
            window = windows[i];

            for (j = 0; j < window.views.length; j++) {
                view = window.views[j]

                // Restore saved properties
                if (view.savedProperties.saved) {
                    view.x = view.savedProperties.x;
                    view.y = view.savedProperties.y;
                    view.savedProperties.saved = false;
                }

                // Disable animations
                view.animationsEnabled = false;
            }
        }
    }
}
