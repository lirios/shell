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
import QtCompositor 1.0
import GreenIsland 1.0
import "decoration"

WaylandWindow {
    property bool animationsEnabled: false
    property alias savedProperties: saved

    id: clientWindow
    objectName: "clientWindow"

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

    Connections {
        target: child.surface
        onSizeChanged: setSize()
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

    // Decoration and actual surface
    WindowDecoration {
        id: decoration
        anchors.fill: parent
        source: child
        onClose: child.surface.destroySurface()
    }

    function setSize() {
        clientWindow.width = child.surface.size.width + decoration.additionalWidth;
        clientWindow.height = child.surface.size.height + decoration.additionalHeight;
    }

    function runMapAnimation() {
        // Run map animation depending on window type
        switch (child.surface.windowType) {
        case WaylandQuickSurface.Toplevel:
            if (toplevelAnimation.map)
                toplevelAnimation.map.start();
            break;
        case WaylandQuickSurface.Transient:
            if (transientAnimation.map)
                transientAnimation.map.start();
            break;
        default:
            break;
        }
    }

    function runUnmapAnimation() {
        // Run map animation depending on window type
        switch (child.surface.windowType) {
        case WaylandQuickSurface.Toplevel:
            if (toplevelAnimation.unmap)
                toplevelAnimation.unmap.start();
            break;
        case WaylandQuickSurface.Transient:
            if (transientAnimation.unmap)
                transientAnimation.unmap.start();
            break;
        default:
            break;
        }
    }

    function runDestroyAnimation() {
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
        default:
            break;
        }
    }
}
