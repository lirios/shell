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
import Hawaii.Themes 1.0 as Themes

Item {
    property int workspaceIndex

    id: workspace
    state: "normal"
    states: [
        State {
            name: "normal"
            StateChangeScript {
                name: "restore"
                script: restore()
            }
        },
        State {
            name: "present"
            StateChangeScript {
                name: "present"
                script: present()
            }
        },
        State {
            name: "reveal"
            StateChangeScript {
                name: "reveal"
                script: reveal()
            }
        }
    ]
    transitions: [
        Transition {
            to: "normal"
            ScriptAction { scriptName: "restore" }
        },
        Transition {
            to: "present"
            ScriptAction { scriptName: "present" }
        },
        Transition {
            to: "reveal"
            ScriptAction { scriptName: "reveal" }
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

    function restore() {
        var windows = windowManager.windowsForOutput(output);
        var i, j, window, view;
        for (i = 0; i < windows.length; i++) {
            window = windows[i];

            for (j = 0; j < window.views.length; j++) {
                // Get a hold of the view
                view = window.views[j];

                // Restore saved properties
                if (view.savedProperties.saved) {
                    view.x = view.savedProperties.x;
                    view.y = view.savedProperties.y;
                    view.scale = view.savedProperties.scale;
                    view.visible = true;
                    view.savedProperties.saved = false;
                }

                // Disable animations
                view.animationsEnabled = false;
            }
        }
    }

    function present() {
        // Parameters
        var windows = windowManager.windowsForOutput(output);
        var hmargin = Themes.Units.dp(48);
        var vmargin = Themes.Units.dp(48);

        // Calculate rows and columns
        var columns = Math.ceil(Math.sqrt(windows.length));
        var rows = Math.ceil(windows.length / columns);

        // Calculate cell size
        var cellWidth = output.availableGeometry.width / columns;
        var cellHeight = output.availableGeometry.height / rows;

        // Loop over windows
        var i, j, window, view, x, y;
        var currentDim = 0, lastDim = 1, row = 0, column = 0;
        for (i = 0; i < windows.length; i++) {
            // Get a hold of the window
            window = windows[i];

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

            // Determine global coordinates
            x = output.position.x + (cellWidth * column);
            y = output.position.y + (cellHeight * row);

            for (j = 0; j < window.views.length; j++) {
                // Get a hold of the view
                view = window.views[j];

                // Enable animations
                view.animationsEnabled = true;

                // Save original properties
                if (!view.savedProperties.saved) {
                    view.savedProperties.x = view.x;
                    view.savedProperties.y = view.y;
                    view.savedProperties.scale = view.scale;
                    view.savedProperties.saved = true;
                }

                // Move this view in output coordinates space
                view.x = x - view.output.position.x;
                view.y = y - view.output.position.y;

                // Scale view down
                view.scale = Math.min((cellWidth - hmargin) / view.width,
                                      (cellHeight - vmargin) / view.height) * 0.98;

                // Hide on other outputs
                if (view.output !== output)
                    view.visible = false;
            }

            // Create a chrome
            /*
            if (!window.chrome) {
                var chromeComponent = Qt.createComponent("WindowChrome.qml");
                window.chrome = chromeComponent.createObject(window, {"window": window});
                window.chrome.clicked.connect(function(w) {
                    w.savedProperties.bringToFront = true;
                    hawaiiCompositor.endEffect("PresentWindowsGrid");
                });
            }
            */
        }
    }

    function reveal() {
        var windows = windowManager.windowsForOutput(output);
        var margin = Themes.Units.dp(96);

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
                    view.savedProperties.scale = view.scale;
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
    }
}
