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
        var num = children.length;

        var i, window;
        for (i = 0; i < num; i++) {
            window = children[i];

            // Restore saved properties
            if (window.savedProperties.saved) {
                window.x = window.savedProperties.x;
                window.y = window.savedProperties.y;
                window.width = window.savedProperties.width;
                window.height = window.savedProperties.height;
                window.scale = window.savedProperties.scale;
                window.savedProperties.saved = false;
            }

            // Disable animations
            window.animationsEnabled = false;
        }
    }

    function present() {
        // Parameters
        var num = children.length;
        var hmargin = Themes.Units.dp(48);
        var vmargin = Themes.Units.dp(48);

        // Calculate rows and columns
        var columns = Math.ceil(Math.sqrt(num));
        var rows = Math.ceil(num / columns);

        // Calculate cell size
        var cellWidth = output.availableGeometry.width / columns;
        var cellHeight = output.availableGeometry.height / rows;

        // Loop over windows
        var i, window, scale;
        var currentDim = 0, lastDim = 1, row = 0, column = 0;
        for (i = 0; i < num; i++) {
            // Find window
            window = children[i];
            /*
            if (!isWindowFine(window))
                continue;
            */

            // Enable animations
            window.animationsEnabled = true;

            // Save original properties
            if (!window.savedProperties.saved) {
                window.savedProperties.x = window.x;
                window.savedProperties.y = window.y;
                window.savedProperties.width = window.width;
                window.savedProperties.height = window.height;
                window.savedProperties.scale = window.scale;
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

            // Move window
            window.x = (cellWidth * column) + hmargin;
            window.y = (cellHeight * row) + vmargin;

            // Resize window
            window.scale = Math.min((cellWidth - hmargin) / window.width,
                                    (cellHeight - vmargin) / window.height) * 0.98;

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
        var num = children.length;
        var margin = Themes.Units.dp(128);

        // Divide screen in 4 zones
        var halfWidth = output.availableGeometry.width * 0.5;
        var halfHeight = output.availableGeometry.height * 0.5;
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

        // Loop over windows
        var i, window, x, y;
        for (i = 0; i < children.length; i++) {
            // Find window
            window = children[i];

            // Skip certain windows
            /*
            if (!isWindowFine(window))
                continue;
            */

            // Enable animations
            window.animationsEnabled = true;

            // Save original properties
            if (!window.savedProperties.saved) {
                window.savedProperties.x = window.x;
                window.savedProperties.y = window.y;
                window.savedProperties.width = window.width;
                window.savedProperties.height = window.height;
                window.savedProperties.scale = window.scale;
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
}
