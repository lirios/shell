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
import "overlays"

WaylandWindow {
    property var chrome: null

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

    // Unresponsive effect
    UnresponsiveOverlay {
        id: unresponsiveEffect
        anchors.fill: parent
        window: clientWindow
    }

    /*
     * Move windows with Super+Drag
     */

    Connections {
        target: compositorRoot
        onKeyPressed: {
            if (event.modifiers & Qt.MetaModifier) {
                dnd.enabled = true;
                rotateMouseArea.enabled = true;
            }
            event.accepted = false;
        }
        onKeyReleased: {
            dnd.enabled = false;
            rotateMouseArea.enabled = false;
            event.accepted = false;
        }
    }

    MouseArea {
        id: dnd
        anchors.fill: parent
        acceptedButtons: enabled ? Qt.LeftButton : Qt.NoBrush
        drag {
            target: clientWindow
            axis: Drag.XAndYAxis
            maximumX: clientWindow.parent.width
            maximumY: clientWindow.parent.height
        }
        enabled: false
        z: 1000000
    }

    /*
     * Rotate with Super+RightButton
     */

    MouseArea {
        id: rotateMouseArea
        anchors.fill: parent
        acceptedButtons: enabled ? Qt.RightButton : Qt.NoButton
        onPositionChanged: {
            var dx = mouse.x - (clientWindow.width * 0.5);
            var dy = mouse.y - (clientWindow.height * 0.5);
            var r = Math.sqrt(dx * dx, dy * dy);
            if (r > 20)
                clientWindow.rotation = Math.atan2(dx / r, -dy / r) * 50;
        }
        enabled: false
        z: 1000000
    }
}
