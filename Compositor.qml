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
import GreenIsland.Core 1.0
import "WindowManagement.js" as WindowManagement
import "screen"

Item {
    readonly property alias screenViews: screenViews
    readonly property var currentScreenView: null

    readonly property alias surfaceModel: surfaceModel

    id: compositorRoot

    Timer {
        id: idleTimer
        interval: compositor.idleInterval
        onIntervalChanged: {
            if (running)
                restart();
        }
    }

    ListModel {
        id: surfaceModel
    }

    Connections {
        target: compositor
        onIdleInhibitResetRequested: compositor.idleInhibit = 0
        onIdleTimerStartRequested: idleTimer.running = true
        onIdleTimerStopRequested: idleTimer.running = false
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
        }
        onFadeOut: {
            // Fade the desktop out
            splash.opacity = 1.0;
        }
        onUnlocked: {
            // Fade the desktop in
            splash.opacity = 0.0;
        }
        onReady: {
            // Fade the desktop in
            splash.opacity = 0.0;

            // Start idle timer
            idleTimer.running = true
        }
        onWorkspaceAdded: {
            // Add a new workspace
            var i;
            for (i = 0; i < screenViews.count; i++)
                screenViews.itemAt(i).workspacesView.add();
        }
        onWorkspaceRemoved: {
            // Remove workspace
            var i;
            for (i = 0; i < screenViews.count; i++)
                screenViews.itemAt(i).workspacesView.remove(index);
        }
        onWorkspaceSelected: {
            // Select workspace
            var i;
            for (i = 0; i < screenViews.count; i++)
                screenViews.itemAt(i).workspacesView.select(index);
        }
        onSurfaceMapped: {
            // A surface was mapped
            WindowManagement.surfaceMapped(surface);
        }
        onSurfaceUnmapped: {
            // A surface was unmapped
            WindowManagement.surfaceUnmapped(surface);
        }
        onSurfaceDestroyed: {
            // A surface was destroyed
            WindowManagement.surfaceDestroyed(surface);
        }
    }

    /*
     * Components
     */

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

    // A screen view for each screen
    // TODO: onItemAdded -> create all workspaces on new screen
    Repeater {
        id: screenViews
        model: screenModel
        z: 998
        onItemRemoved: {
            // First find the new primary output or fallback to the last available
            var i, screenView = null;
            for (i = 0; i < screenViews.count; i++) {
                screenView = screenViews.itemAt(i);
                if (screenView.primary)
                    break;
            }

            // Move window to the new primary output
            var j;
            for (j = 0; j < item.currentWorkspace.children.length; j++) {
                var window = item.currentWorkspace.children[j];

                // Skip children that are not application windows
                if (window.objectName !== "clientWindow")
                    continue;

                // Reparent window
                window.parent = screenView.currentWorkspace;
            }
        }

        ScreenView {
            name: model.name
            primary: model.primary
            x: model.geometry.x
            y: model.geometry.y
            width: model.geometry.width
            height: model.geometry.height
            rotation: model.rotation
            transformOrigin: Item.Center
        }

        function screenViewForCoordinates(x, y) {
            var i;
            for (i = 0; i < screenViews.count; i++) {
                var screenView = screenViews.itemAt(i);
                var geometry = Qt.rect(screenView.x, screenView.y,
                                       screenView.width, screenView.height);

                var l = geometry.x;
                var r = geometry.x + geometry.width;
                var t = geometry.y;
                var b = geometry.y + geometry.height;

                if (x < l || x > r)
                    continue;
                if (y < t || y > b)
                    continue;
                return screenView;
            }

            return null;
        }
    }

    /*
     * Methods
     */

    function toggleEffect(name) {
        var i;
        for (i = 0; i < screenViews.count; i++)
            screenViews.itemAt(i).workspacesView.currentWorkspace.effects.toggle(name);
    }
}
