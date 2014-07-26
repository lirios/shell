/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtCompositor 1.0
import GreenIsland 1.0
import "overlays"

WaylandWindow {
    property var chrome: null
    property bool animationsEnabled: false
    property alias savedProperties: saved

    id: clientWindow
    objectName: "clientWindow"
    states: [
        State {
            name: "focused"
            when: child.focus && !clientWindow.unresponsive
            PropertyChanges { target: unresponsiveEffect; opacity: 0.0 }
        },
        State {
            name: "unfocused"
            when: !child.focus && !clientWindow.unresponsive
            PropertyChanges { target: unresponsiveEffect; opacity: 0.0 }
        },
        State {
            name: "focused-unresponsive"
            when: child.focus && clientWindow.unresponsive
            PropertyChanges { target: unresponsiveEffect; opacity: 1.0 }
        },
        State {
            name: "unfocused-unresponsive"
            when: !child.focus && clientWindow.unresponsive
            PropertyChanges { target: unresponsiveEffect; opacity: 1.0 }
        }
    ]

    QtObject {
        id: saved

        property real x
        property real y
        property real z
        property real scale

        property var chrome
    }

    Behavior on x {
        enabled: clientWindow.animationsEnabled

        NumberAnimation {
            easing.type: Easing.InCubic
            duration: 250
        }
    }

    Behavior on y {
        enabled: clientWindow.animationsEnabled

        NumberAnimation {
            easing.type: Easing.InQuad
            duration: 250
        }
    }

    Behavior on width {
        enabled: clientWindow.animationsEnabled

        NumberAnimation {
            easing.type: Easing.InCubic
            duration: 250
        }
    }

    Behavior on height {
        enabled: clientWindow.animationsEnabled

        NumberAnimation {
            easing.type: Easing.InCubic
            duration: 250
        }
    }

    Behavior on scale {
        enabled: clientWindow.animationsEnabled

        NumberAnimation {
            easing.type: Easing.InQuad
            duration: 250
        }
    }

    Behavior on opacity {
        enabled: clientWindow.animationsEnabled

        NumberAnimation {
            easing.type: Easing.Linear
            duration: 250
        }
    }

    ToplevelWindowAnimation {
        id: toplevelAnimation
        windowItem: clientWindow
    }

    TransientWindowAnimation {
        id: transientAnimation
        windowItem: clientWindow
    }

    PopupWindowAnimation {
        id: popupAnimation
        windowItem: clientWindow
    }

    Connections {
        target: child.surface
        onUnmapped: runUnmapAnimation()
    }

    Connections {
        target: child
        onSurfaceDestroyed: runDestroyAnimation()
    }

    // Decrease contrast for transient parents
    ContrastEffect {
        id: contrast
        anchors.fill: parent
        source: clientWindow
        blend: transientChildren ? 0.742 : 1.0
        color: "black"
        visible: transientChildren != null

        Behavior on blend {
            NumberAnimation {
                easing.type: transientChildren ? Easing.InQuad : Easing.OutQuad
                duration: 250
            }
        }
    }

    // Unresponsive effect
    UnresponsiveOverlay {
        id: unresponsiveEffect
        anchors.fill: parent
        window: clientWindow
    }

    function runMapAnimation() {
        // Disable behavior animations temporarily
        clientWindow.animationsEnabled = false;

        // Run map animation depending on window type
        switch (child.surface.windowType) {
        case WaylandQuickSurface.Toplevel:
            if (toplevelAnimation.map) {
                toplevelAnimation.map.start();
                toplevelAnimation.mapAnimationStopped.connect(function() {
                    clientWindow.animationsEnabled = true;
                });
            }
            break;
        case WaylandQuickSurface.Transient:
            if (transientAnimation.map) {
                transientAnimation.map.start();
                transientAnimation.mapAnimationStopped.connect(function() {
                    clientWindow.animationsEnabled = true;
                });
            }
            break;
        case WaylandQuickSurface.Popup:
            if (popupAnimation.map) {
                popupAnimation.map.start();
                popupAnimation.mapAnimationStopped.connect(function() {
                    clientWindow.animationsEnabled = true;
                });
            }
            break;
        default:
            break;
        }
    }

    function runUnmapAnimation() {
        // Disable behavior animations temporarily
        clientWindow.animationsEnabled = false;

        // Run map animation depending on window type
        switch (child.surface.windowType) {
        case WaylandQuickSurface.Toplevel:
            if (toplevelAnimation.unmap) {
                toplevelAnimation.unmap.start();
                toplevelAnimation.unmapAnimationStopped.connect(function() {
                    clientWindow.animationsEnabled = true;
                });
            }
            break;
        case WaylandQuickSurface.Transient:
            if (transientAnimation.unmap) {
                transientAnimation.unmap.start();
                transientAnimation.unmapAnimationStopped.connect(function() {
                    clientWindow.animationsEnabled = true;
                });
            }
            break;
        case WaylandQuickSurface.Popup:
            if (popupAnimation.unmap) {
                popupAnimation.unmap.start();
                popupAnimation.unmapAnimationStopped.connect(function() {
                    clientWindow.animationsEnabled = true;
                });
            }
            break;
        default:
            break;
        }
    }

    function runDestroyAnimation() {
        // Disable behavior animations
        clientWindow.animationsEnabled = false;

        // Run destroy animation depending on window type
        switch (child.surface.windowType) {
        case WaylandQuickSurface.Toplevel:
            if (toplevelAnimation.destroy)
                toplevelAnimation.destroy.start();
            break;
        case WaylandQuickSurface.Transient:
            if (transientAnimation.destroy)
                transientAnimation.destroy.start();
            break;
        case WaylandQuickSurface.Popup:
            if (popupAnimation.destroy)
                popupAnimation.destroy.start();
            break;
        default:
            break;
        }
    }
}
