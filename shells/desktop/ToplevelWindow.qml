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
import org.kde.plasma.core 2.0 as PlasmaCore

WindowWrapper {
    property var chrome: null
    property var popupChild: null
    property var transientChildren: null
    property bool animationsEnabled: false
    property alias savedProperties: saved

    id: window
    objectName: "clientWindow"
    animation: ToplevelWindowAnimation {
        windowItem: window
    }

    // Decrease contrast for transient parents
    ContrastEffect {
        id: contrast
        anchors.fill: parent
        source: window
        blend: transientChildren ? 0.742 : 1.0
        color: "black"
        z: visible ? 2 : 0
        visible: transientChildren != null

        Behavior on blend {
            NumberAnimation {
                easing.type: transientChildren ? Easing.InQuad : Easing.OutQuad
                duration: units.shortDuration
            }
        }
    }

    // Dim windows when not focused
    ContrastEffect {
        id: dimEffect
        anchors.fill: parent
        source: window
        blend: 0.742
        color: "gray"
        z: visible ? 2 : 0
        visible: !child.focus && !popupChild
    }

    // Connect to surface or child events
    Connections {
        target: child
        onRaiseRequested: compositorRoot.moveFront(window)
    }

    /*
     * Position and scale
     */

    QtObject {
        id: saved

        property real x
        property real y
        property real z
        property real scale
        property var chrome
        property bool bringToFront: false
        property bool saved: false
    }

    /*
     * Animations
     */

    Behavior on x {
        enabled: animationsEnabled

        NumberAnimation {
            easing.type: Easing.InCubic
            duration: units.longDuration
        }
    }

    Behavior on y {
        enabled: animationsEnabled

        NumberAnimation {
            easing.type: Easing.InQuad
            duration: units.longDuration
        }
    }

    Behavior on width {
        enabled: animationsEnabled

        NumberAnimation {
            easing.type: Easing.InCubic
            duration: units.shortDuration
        }
    }

    Behavior on height {
        enabled: animationsEnabled

        NumberAnimation {
            easing.type: Easing.InCubic
            duration: units.shortDuration
        }
    }

    Behavior on scale {
        enabled: animationsEnabled

        NumberAnimation {
            easing.type: Easing.InQuad
            duration: units.shortDuration
        }
    }

    Behavior on opacity {
        enabled: animationsEnabled

        NumberAnimation {
            easing.type: Easing.Linear
            duration: units.shortDuration
        }
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

    // FIXME: Dragging doesn't change child.surface.globalPosition
    // resulting in the window not being shown in other outputs
    MouseArea {
        id: dnd
        anchors.fill: parent
        acceptedButtons: enabled ? Qt.LeftButton : Qt.NoBrush
        drag {
            target: window
            axis: Drag.XAndYAxis
            maximumX: window.parent ? window.parent.width : 0
            maximumY: window.parent ? window.parent.height : 0
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
            var dx = mouse.x - (window.width * 0.5);
            var dy = mouse.y - (window.height * 0.5);
            var r = Math.sqrt(dx * dx, dy * dy);
            if (r > 20)
                window.rotation = Math.atan2(dx / r, -dy / r) * 50;
        }
        enabled: false
        z: 1000000
    }
}
