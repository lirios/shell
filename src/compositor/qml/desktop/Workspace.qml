// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Liri.private.shell 1.0 as P
import "PresentEffect.js" as PresentEffect
import "../windows"

Item {
    id: workspace

    readonly property alias shellSurfaces: shellSurfaces
    readonly property alias xwaylandShellSurfaces: xwaylandShellSurfaces

    signal effectStarted(string effect)
    signal effectStopped(string effect)

    width: parent.width
    height: parent.height

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
    onEffectStopped: {
        workspace.state = "normal";
    }
    onChildrenChanged: {
        switch (state) {
        case "present":
            workspace.effectStopped(state);
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

    ListModel {
        id: shellSurfaces
    }

    ListModel {
        id: xwaylandShellSurfaces
    }

    Repeater {
        model: shellSurfaces

        Chrome {
            id: chrome

            window: model.window
            shellSurface: model.shellSurface
            output: model.output
            shellSurfaceType: P.ChromeItem.WaylandShellSurface

            onDestroyAnimationFinished: {
                // Remove from shell surface model
                shellSurfaces.remove(index);

                // Destroy the window object but do it only once to avoid
                // freeing already freed memory
                if (chrome.primary)
                    chrome.window.destroy();
            }
        }
    }

    Repeater {
        model: xwaylandShellSurfaces

        Chrome {
            id: xwaylandChrome

            window: model.window
            shellSurface: model.shellSurface
            output: model.output
            shellSurfaceType: P.ChromeItem.XWaylandShellSurface

            onDestroyAnimationFinished: {
                // Remove from shell surface model
                xwaylandShellSurfaces.remove(index);

                // Destroy the window object but do it only once to avoid
                // freeing already freed memory
                if (xwaylandChrome.primary)
                    xwaylandChrome.window.destroy();
            }
        }
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
        for (var i = 0; i < workspace.visibleChildren.length; i++) {
            // Get a hold of the view
            var view = workspace.visibleChildren[i];

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
        for (var i = 0; i < workspace.visibleChildren.length; i++) {
            // Get a hold of the view
            var view = workspace.visibleChildren[i];

            var pos = __private.storage[view];
            if (pos !== undefined) {
                view.moveItem.animateTo(pos.x, pos.y);
                delete __private.storage[view];
            }
        }

        workspace.effectStopped("reveal");
    }

    function stopEffect(effect) {
        if (state === effect)
            workspace.effectStopped(effect);
    }
}
