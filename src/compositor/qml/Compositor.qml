/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import "CompositorLogic.js" as CompositorLogic

Item {
    id: root

    // Currently selected window
    property variant selectedWindow: null

    // True if we're using the normal layout
    property bool normalLayout: true

    // Relayout windows
    onWidthChanged: CompositorLogic.relayout()
    onHeightChanged: CompositorLogic.relayout()

    // Bind compositor signals
    Connections {
        target: compositor
        onReady: splash.opacity = 0.0
    }

    // Black rectangle for fade-in and fade-out effects
    Rectangle {
        id: splash
        color: "black"
        x: 0
        y: 0
        z: 1000
        width: 8192
        height: 8192
        opacity: 1.0

        Behavior on opacity {
            NumberAnimation { duration: 250 }
        }
    }

    /*
     * Slots
     */

    function shellWindowAdded(window) {
        CompositorLogic.shellWindowAdded(window);
    }

    function windowAdded(window) {
        CompositorLogic.windowAdded(window);
    }

    function windowResized(window) {
        CompositorLogic.windowResized(window);
    }

    function windowDestroyed(window) {
        CompositorLogic.windowDestroyed(window);
    }

    function removeWindow(window) {
        CompositorLogic.windowRemoved(compositor, window);
    }
}
