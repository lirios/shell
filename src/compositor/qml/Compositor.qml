/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import GreenIsland 1.0
import Hawaii.Shell 0.1
import "WindowManager.js" as WindowManager

Item {
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

    // Modal dialog overlay
    // TODO: Create a surface every time a modal dialog is created
    Rectangle {
        id: modalOverlay
        color: "black"
        x: 0
        y: 0
        z: 998
        width: 8192
        height: 8192
        opacity: 0.0

        Behavior on opacity {
            NumberAnimation { duration: 250 }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            preventStealing: true
            onClicked: mouse.accepted = false
        }
    }

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

    function showModalOverlay() {
        modalOverlay.opacity = 0.7;
    }

    function hideModalOverlay() {
        modalOverlay.opacity = 0.0;
    }

    function shellWindowMapped(window) {
        WindowManager.shellWindowMapped(window);
    }

    function shellWindowUnmapped(window) {
        WindowManager.shellWindowUnmapped(window);
    }

    function shellWindowDestroyed(window) {
        WindowManager.shellWindowDestroyed(window);
    }

    function windowAdded(window) {
        WindowManager.windowAdded(window);
    }

    function windowUnmapped(window) {
        WindowManager.windowUnmapped(window);
    }

    function windowDestroyed(window) {
        WindowManager.windowDestroyed(window);
    }

    function removeWindow(window) {
        WindowManager.windowRemoved(window);
    }
}
