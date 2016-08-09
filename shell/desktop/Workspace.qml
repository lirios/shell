/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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
import Fluid.Controls 1.0

Item {
    readonly property alias animateWindows: __private.animationsEnabled

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

    QtObject {
        id: __private

        property var storage: ({})
        property var chromes: ({})
        property bool animationsEnabled: false
    }

    Component {
        id: chromeComponent

        PresentWindowsChrome {}
    }

    function windowsList() {
        var windows = [];
        var i, window;
        for (i = 0; i < applicationManager.windows.length; i++) {
            window = applicationManager.windows[i];
            if (window.designedOutput === output)
                windows.push(window);
        }
        return windows;
    }

    function present() {
        console.time("present " + output.model);

        // Enable window animations
        __private.animationsEnabled = true;

        // Parameters
        var windows = windowsList();
        var grid = Math.ceil(windows.length / 2);
        var maxWidth = Math.floor(output.availableGeometry.width / grid);
        var maxHeight = Math.floor(output.availableGeometry.height / 2);
        var scale, x, y;

        // Loop over the windows
        var i, j, window;
        for (i = 0; i < windows.length; i++) {
            // Get a hold of the window
            window = windows[i];

            // Window size
            var width = window.surface.size.width;
            var height = window.surface.size.height;

            // Calculate scale factor
            if (width >= height)
                scale = width > maxWidth ? maxWidth / width : 0.85;
            else
                scale = height > maxHeight ? maxHeight / height : 0.85;

            // Add a little margin
            scale -= 0.15;

            // Calculate position
            x = ((i % grid) * maxWidth) + Math.floor((maxWidth - scale * width) / 2);
            y = ((i < grid ? 0 : 1) * maxHeight) + Math.floor((maxHeight - scale * height) / 2);

            // Save window position
            __private.storage[window] = {"x": window.moveItem.x, "y": window.moveItem.y};

            // Move the window
            window.moveItem.x = output.position.x + x;
            window.moveItem.y = output.position.y + y;

            // Scale shell surface items down
            for (j = 0; j < window.views.length; j++) {
                window.views[j].sizeFollowsSurface = false;
                window.views[j].inputEventsEnabled = false;
                window.views[j].scale = scale;

                // Create the chrome
                var chrome = chromeComponent.createObject(window.views[j], {"window": window});
                __private.chromes[window.views[j]] = chrome;

                // Handle chrome events
                chrome.activated.connect(function(window) {
                    state = "normal";
                    window.activate();
                });
                chrome.closeRequested.connect(function(window) {
                    state = "normal";
                    window.close();
                });
            }
        }
        console.timeEnd("present loop " + output.model);

        console.timeEnd("present " + output.model);
    }

    function presentRestore() {
        // Restore windows
        var windows = windowsList();
        var i, j, window, view;
        for (i = 0; i < windows.length; i++) {
            // Get a hold of the window
            window = windows[i];

            // Restore position
            var pos = __private.storage[window];
            window.moveItem.x = pos.x;
            window.moveItem.y = pos.y;
            delete __private.storage[window];

            // Scale shell surface items up
            for (j = 0; j < window.views.length; j++) {
                window.views[j].sizeFollowsSurface = true;
                window.views[j].inputEventsEnabled = true;
                window.views[j].scale = 1.0;

                // Destroy the chrome
                __private.chromes[window.views[j]].destroy();
                delete __private.chromes[window.views[j]];
            }
        }

        // Disable window animations
        __private.animationsEnabled = false;
    }

    function reveal() {
        console.time("reveal " + output.model);

        // Enable window animations
        __private.animationsEnabled = true;

        var windows = windowsList();
        var margin = 96;

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

            __private.storage[window] = {"x": window.moveItem.x, "y": window.moveItem.y};
            window.moveItem.x = x;
            window.moveItem.y = y;
        }
        console.timeEnd("reveal loop " + output.model);

        console.timeEnd("reveal " + output.model);
    }

    function revealRestore() {
        // Restore windows position
        var windows = windowsList();
        var i, j, window, view;
        for (i = 0; i < windows.length; i++) {
            window = windows[i];

            var pos = __private.storage[window];
            window.moveItem.x = pos.x;
            window.moveItem.y = pos.y;
            delete __private.storage[window];
        }

        // Disable window animations
        __private.animationsEnabled = false;
    }
}
