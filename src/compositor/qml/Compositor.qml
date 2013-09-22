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

Rectangle {
    id: root
    color: "black"
    opacity: 0.0

    // Currently selected window
    property variant selectedWindow: null

    // True if we're using the normal layout
    property bool normalLayout: true

    // Relayout windows
    onWidthChanged: CompositorLogic.relayout()
    onHeightChanged: CompositorLogic.relayout()

    states: [
        State {
            name: "shown"
            when: opacity == 1.0
        },
        State {
            name: "hidden"
            when: opacity == 0.0
        }
    ]

    transitions: [
        Transition {
            to: "shown"
            NumberAnimation { properties: "opacity"; duration: 7000 }
        },
        Transition {
            to: "hidden"
            NumberAnimation { properties: "opacity"; duration: 7000 }
        }
    ]

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

    Component.onCompleted: opacity = 1.0
}
