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
import WaylandCompositor 1.0
import GreenIsland 1.0
import Hawaii.Shell 0.1
import "WindowManager.js" as WindowManager

Item {
    property alias layers: layers

    id: root

    // Bind compositor signals
    Connections {
        target: compositor
        onIdle: {
            // Fade the desktop out
            splash.opacity = 1.0;

            // Lock the session
            compositor.lockSession();
        }
        onWake: {
            // Unlock the session
            compositor.unlockSession();
        }
        onFadeIn: {
            // Fade the desktop in
            splash.opacity = 0.0;

            // Damage all surfaces
            compositor.damageAll();
        }
        onFadeOut: {
            // Fade the desktop out
            splash.opacity = 1.0;
        }
        onUnlocked: {
            // Fade the desktop in
            splash.opacity = 0.0;

            // Damage all surfaces
            compositor.damageAll();
        }
        onReady: {
            // Fade the desktop in
            splash.opacity = 0.0;

            // Start idle timer
            compositor.startIdleTimer();
        }
    }

    // FPS counter
    Text {
        anchors {
            top: parent.top
            right: parent.right
        }
        z: 1000
        text: fpsCounter.fps
        font.pointSize: 36
        style: Text.Raised
        styleColor: "#222"
        color: "white"
        visible: false

        FpsCounter {
            id: fpsCounter
        }
    }

    // Black rectangle for fade-in and fade-out effects
    Rectangle {
        id: splash
        anchors.fill: parent
        color: "black"
        z: 999

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 250
            }
        }
    }

    // Layers for windows
    Layers {
        id: layers
        anchors.fill: parent
        z: 998
    }

    function windowAdded(window) {
        WindowManager.windowAdded(window);
    }

    function windowRemoved(window) {
        WindowManager.windowRemoved(window);
    }

    function windowDestroyed(window) {
        WindowManager.windowDestroyed(window);
    }
}
