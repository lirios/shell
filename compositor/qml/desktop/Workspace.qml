/****************************************************************************
 * This file is part of Liri.
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

    QtObject {
        id: __private

        property var storage: ({})
        property var chromes: ({})
    }

    Component {
        id: chromeComponent

        PresentWindowsChrome {}
    }

    function present() {
        console.time("present " + output.model);

        // Parameters
        var grid = Math.ceil(compositor.shellSurfaces.count / 2);
        var maxWidth = Math.floor(output.availableGeometry.width / grid);
        var maxHeight = Math.floor(output.availableGeometry.height / 2);

        // Loop over the windows
        for (var i = 0; i < compositor.shellSurfaces.count; i++) {
            // Get a hold of the shell surface
            var shellSurface = compositor.shellSurfaces.get(i).shellSurface;
            var view = output.viewsBySurface[shellSurface.surface];

            // Window size
            var width = view.width;
            var height = view.height;

            // Calculate scale factor
            var scale;
            if (width >= height)
                scale = width > maxWidth ? maxWidth / width : 0.85;
            else
                scale = height > maxHeight ? maxHeight / height : 0.85;

            // Add a little margin
            scale -= 0.15;

            // Calculate position
            var x = ((i % grid) * maxWidth) + Math.floor((maxWidth - scale * width) / 2);
            var y = ((i < grid ? 0 : 1) * maxHeight) + Math.floor((maxHeight - scale * height) / 2);

            // Save window position
            __private.storage[view] = {"x": view.moveItem.x, "y": view.moveItem.y};

            // Move the window
            view.moveItem.animateTo(output.position.x + x, output.position.y + y);

            if (view.primary) {
                // Scale view down and disable input
                view.inputEventsEnabled = false;
                view.resizeTo(view.width * scale, view.height * scale);

                // Create the chrome
                var chrome = chromeComponent.createObject(view, {"view": view});
                __private.chromes[view] = chrome;

                // Handle chrome events
                chrome.selected.connect(function(view) {
                    state = "normal";
                    view.activate();
                });
                chrome.closed.connect(function(view) {
                    state = "normal";
                    view.close();
                });
            } else {
                // Hide view because it's not on this output
                view.visible = false;
            }
        }
        console.timeEnd("present loop " + output.model);

        console.timeEnd("present " + output.model);
    }

    function presentRestore() {
        // Restore windows
        for (var i = 0; i < compositor.shellSurfaces.count; i++) {
            // Get a hold of the shell surface
            var shellSurface = compositor.shellSurfaces.get(i).shellSurface;
            var view = output.viewsBySurface[shellSurface.surface];

            // Restore position
            var pos = __private.storage[view];
            if (pos !== undefined) {
                view.moveItem.animateTo(pos.x, pos.y);
                delete __private.storage[view];
            }

            // Scale view up and enable input
            view.inputEventsEnabled = true;
            view.restoreSize();
            view.visible = true;

            // Destroy chrome
            var chrome = __private.chromes[view];
            if (chrome !== undefined) {
                chrome.destroy();
                delete __private.chromes[view];
            }
        }
    }

    function reveal() {
        console.time("reveal " + output.model);

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
        var x, y;
        for (var i = 0; i < compositor.shellSurfaces.count; i++) {
            // Get a hold of the shell surface
            var shellSurface = compositor.shellSurfaces.get(i).shellSurface;
            var view = output.viewsBySurface[shellSurface.surface];

            // Skip shell surfaces not rendered on this output
            if (!view.primary)
                continue;

            // Determine global coordinates to the closest of the 4 zones
            if (contains(topRight, view.x, view.y)) {
                x = topRight.x + topRight.width - margin;
                y = topRight.y - view.height + margin;
            } else if (contains(bottomLeft, view.x, view.y)) {
                x = bottomLeft.x - view.width + margin;
                y = bottomLeft.y + bottomLeft.height - margin;
            } else if (contains(bottomRight, view.x, view.y)) {
                x = bottomRight.x + bottomRight.width - margin;
                y = bottomRight.y + bottomRight.height - margin;
            } else {
                x = topLeft.x - view.width + margin;
                y = topLeft.y - view.height + margin;
            }

            __private.storage[view] = {"x": view.moveItem.x, "y": view.moveItem.y};
            view.moveItem.animateTo(x, y);
        }
        console.timeEnd("reveal loop " + output.model);

        console.timeEnd("reveal " + output.model);
    }

    function revealRestore() {
        // Restore windows position
        for (var i = 0; i < compositor.shellSurfaces.count; i++) {
            // Get a hold of the shell surface
            var shellSurface = compositor.shellSurfaces.get(i).shellSurface;
            var view = output.viewsBySurface[shellSurface.surface];

            var pos = __private.storage[view];
            if (pos !== undefined) {
                view.moveItem.animateTo(pos.x, pos.y);
                delete __private.storage[view];
            }
        }
    }
}
