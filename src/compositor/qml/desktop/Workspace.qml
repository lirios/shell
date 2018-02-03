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
import "PresentEffect.js" as PresentEffect

Item {
    id: workspace

    signal effectStarted(string effect)
    signal effectStopped(string effect)

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
            for (var i = 0; i < liriCompositor.screenManager.count; i++)
                liriCompositor.screenManager.objectAt(i).surfacesArea.state = "normal";
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

        workspace.effectStarted("present");

        PresentEffect.spreadWindows();

        console.timeEnd("present " + output.model);
    }

    function presentRestore() {
        PresentEffect.restoreWindows();

        workspace.effectStopped("present");
    }

    function reveal() {
        console.time("reveal " + output.model);

        workspace.effectStarted("reveal");

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
        for (var i = 0; i < liriCompositor.shellSurfaces.count; i++) {
            // Get a hold of the shell surface
            var shellSurface = liriCompositor.shellSurfaces.get(i).shellSurface;
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
        for (var i = 0; i < liriCompositor.shellSurfaces.count; i++) {
            // Get a hold of the shell surface
            var shellSurface = liriCompositor.shellSurfaces.get(i).shellSurface;
            var view = output.viewsBySurface[shellSurface.surface];

            var pos = __private.storage[view];
            if (pos !== undefined) {
                view.moveItem.animateTo(pos.x, pos.y);
                delete __private.storage[view];
            }
        }

        workspace.effectStopped("reveal");
    }
}
