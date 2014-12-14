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
import GreenIsland 1.0
import "decoration"

ApplicationWindow {
    property bool animationsEnabled: false

    id: clientWindow
    objectName: "clientWindow"

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

    PopupWindowAnimation {
        id: popupAnimation
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

    // Decoration and actual surface
    PopupWindowDecoration {
        id: decoration
        anchors.fill: parent
        source: child
    }

    function setSize() {
        clientWindow.width = child.surface.size.width;
        clientWindow.height = child.surface.size.height;
    }

    function runMapAnimation() {
        if (popupAnimation.map)
            popupAnimation.map.start();
    }

    function runUnmapAnimation() {
        if (popupAnimation.unmap)
            popupAnimation.unmap.start();
    }

    function runDestroyAnimation() {
        if (popupAnimation.destroy)
            popupAnimation.destroy.start();
    }
}
