/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
import Hawaii.Themes 1.0 as Themes
import "../decoration"

WindowWrapper {
    property var chrome: null
    property var popupChild: null
    property var transientChildren: null
    property bool animationsEnabled: visible
    property alias savedProperties: saved

    id: window
    objectName: "clientWindow"
    animation: TopLevelWindowAnimation {
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
        onResizeStarted: animationsEnabled = false
        onResizeFinished: animationsEnabled = true
        onActiveChanged: if (clientWindow.active) compositorRoot.moveFront(window)
        onMinimizedChanged: {
            if (clientWindow.minimized) {
                // Save old position and scale
                saved.x = window.x;
                saved.y = window.y;
                saved.scale = window.scale;

                // Move the window
                var panel = compositorRoot.screenView.panel;
                var center = Qt.point(panel.currentLauncherItem.width / 2,
                                      panel.currentLauncherItem.height / 2);
                var pos = compositorRoot.mapFromItem(panel.currentLauncherItem, center.x, center.y);
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
        onWindowMenuRequested: {
            /*
              TODO: Destroy when closed
            var menu = windowMenu.createObject(window, {
                                                   "clientWindow": clientWindow,
                                                   "visualLayer": compositorRoot.screenView.layers.panel,
                                                   "visualParent": window
                                               });
            menu.openAt(pt.x, pt.y);
            */
        }
    }

    /*
    Component {
        id: windowMenu

        WindowMenu {}
    }
    */

/*
    property alias decoration: decoration
    additionalWidth: decoraiton.additionalWidth
    additionalHeight: decoration.additionalHeight
    WindowDecoration {
        id: decoration
        anchors.fill: parent
        clientWindow: window.clientWindow
    }
*/

    /*
     * Position and scale
     */

    QtObject {
        id: saved

        property real x
        property real y
        property real width
        property real height
        property real scale
        property var chrome
        property bool bringToFront: false
        property bool saved: false
    }

    /*
     * Animations
     */

    // NOTE: Never animate z otherwise window stacking won't work
    // because the z property will be changed progressively by
    // the animation

    Behavior on x {
        enabled: animationsEnabled
        SmoothedAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.mediumDuration
        }
    }

    Behavior on y {
        enabled: animationsEnabled
        SmoothedAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.mediumDuration
        }
    }

    Behavior on width {
        enabled: animationsEnabled
        SmoothedAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.mediumDuration
        }
    }

    Behavior on height {
        enabled: animationsEnabled
        SmoothedAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.mediumDuration
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
     * Rotate with Super+RightButton
     */

    /*
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
    */

    /*
     * Component
     */

    Component.onDestruction: {
        // Destroy chrome if any
        if (window.chrome)
            window.chrome.destroy();
    }
}
