/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Themes 1.0 as Themes

WindowWrapper {
    property var chrome: null
    property var popupChild: null
    property var transientChildren: null
    property bool animationsEnabled: visible
    property alias savedProperties: saved

    id: window
    objectName: "clientWindow"
    animation: ToplevelWindowAnimation {
        windowItem: window
    }

    // Decrease contrast for transient parents
    ContrastEffect {
        id: contrast
        x: clientWindow ? clientWindow.internalGeometry.x : 0
        y: clientWindow ? clientWindow.internalGeometry.y : 0
        width: clientWindow ? clientWindow.internalGeometry.width : 0
        height: clientWindow ? clientWindow.internalGeometry.height : 0
        source: window
        blend: transientChildren ? 0.742 : 1.0
        color: "black"
        z: visible ? 2 : 0
        visible: transientChildren != null

        Behavior on blend {
            NumberAnimation {
                easing.type: transientChildren ? Easing.InQuad : Easing.OutQuad
                duration: Themes.Units.shortDuration
            }
        }
    }

    // Dim windows when not focused
    /*
    ContrastEffect {
        id: dimEffect
        x: clientWindow.internalGeometry.x
        y: clientWindow.internalGeometry.y
        width: clientWindow.internalGeometry.width
        height: clientWindow.internalGeometry.height
        source: window
        blend: 0.742
        color: "gray"
        z: visible ? 2 : 0
        visible: !clientWindow.active && !popupChild
    }
    */

    // Connect to the client window
    Connections {
        target: clientWindow
        onMotionStarted: animationsEnabled = false
        onMotionFinished: animationsEnabled = true
        onActiveChanged: if (clientWindow.active) compositorRoot.moveFront(window)
        onMinimizedChanged: {
            if (clientWindow.minimized) {
                // Save old position and scale
                saved.x = window.x;
                saved.y = window.y;
                saved.scale = window.scale;

                // Move the window
                var panel = compositorRoot.screenView.panel;
                var pos = compositorRoot.mapFromItem(panel.currentLauncherItem, 0, 0);
                window.x = pos.x - (width * 0.5);
                window.y = pos.y - (height * 0.5);
                window.scale = 0.0;
                window.opacity = 0.0;
            } else {
                // Restore old properties
                window.x = saved.x;
                window.y = saved.y;
                window.scale = saved.scale;
                window.opacity = 1.0;
            }
        }
    }

    /*
     * Position and scale
     */

    QtObject {
        id: saved

        property real x
        property real y
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
            easing.type: Easing.OutQuad
            duration: Themes.Units.longDuration
        }
    }

    Behavior on y {
        enabled: animationsEnabled
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.longDuration
        }
    }

    Behavior on z {
        enabled: visible
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.shortDuration
        }
    }

    Behavior on width {
        enabled: visible
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.shortDuration
        }
    }

    Behavior on height {
        enabled: visible
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.shortDuration
        }
    }

    Behavior on scale {
        enabled: visible
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.longDuration
        }
    }

    Behavior on opacity {
        enabled: visible
        NumberAnimation {
            easing.type: Easing.Linear
            duration: Themes.Units.longDuration
        }
    }

    /*
     * Move windows with Super+Drag
     */

    Connections {
        target: compositorRoot
        onKeyPressed: {
            if (event.modifiers === Qt.MetaModifier) {
                ////clientWindow.move();
                rotateMouseArea.enabled = true;
            }
            event.accepted = false;
        }
        onKeyReleased: {
            rotateMouseArea.enabled = false;
            event.accepted = false;
        }
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

    /*
     * Component
     */

    Component.onDestruction: {
        // Destroy chrome if any
        if (window.chrome)
            window.chrome.destroy();
    }
}
