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

WaylandWindow {
    id: clientWindow

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

    /*
    MouseArea {
        anchors.fill: parent
        enabled: !clientWindow.focus
        preventStealing: false
        onClicked: {
            // Change stacking order
            var i, clientWindow = waylandWindow.parent.children;
            for (i = clientWindow.length; i >= 0; i--) {
                var curWindow = clientWindow[i];

                if (curWindow !== waylandWindow)
                    curWindow.z = -i;
            }
            clientWindow.z = 1;

            // Give window focus
            clientWindow.takeFocus();
        }
    }
    */

    function runMapAnimation() {
        switch (child.surface.windowType) {
        case WaylandQuickSurface.Toplevel:
            if (toplevelAnimation.map)
                toplevelAnimation.map.start();
            break;
        case WaylandQuickSurface.Transient:
            if (transientAnimation.map)
                transientAnimation.map.start();
            break;
        case WaylandQuickSurface.Popup:
            if (popupAnimation.map)
                popupAnimation.map.start();
            break;
        default:
            break;
        }
    }

    function runUnmapAnimation() {
        switch (child.surface.windowType) {
        case WaylandQuickSurface.Toplevel:
            if (toplevelAnimation.unmap)
                toplevelAnimation.unmap.start();
            break;
        case WaylandQuickSurface.Transient:
            if (transientAnimation.unmap)
                transientAnimation.unmap.start();
            break;
        case WaylandQuickSurface.Popup:
            if (popupAnimation.unmap)
                popupAnimation.unmap.start();
            break;
        default:
            break;
        }
    }

    function runDestroyAnimation() {
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
