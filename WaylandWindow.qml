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

Item {
    property var child
    property bool unresponsive: false
    property var role: child.surface.windowProperties.role
    property var transientChildren: null

    id: waylandWindow
    opacity: 1.0
    onVisibleChanged: {
        if (child)
            child.surface.clientRenderingEnabled = visible;
    }

    SurfaceRenderer {
        anchors.fill: parent
        source: child
    }

    Connections {
        target: child.surface
        onSizeChanged: {
            waylandWindow.width = child.surface.size.width;
            waylandWindow.height = child.surface.size.height;
        }
        onPong: {
            // Surface replied with a pong this means it's responsive
            pingPongTimer.running = false;
            unresponsive = false;
        }
    }

    Timer {
        id: pingPongTimer
        interval: 200
        onTriggered: unresponsive = true
    }

    function pingSurface() {
        // Ping logic applies only to windows actually visible
        if (!visible)
            return;

        // Ping the surface to see whether it's responsive, if a pong
        // doesn't arrive before the timeout is trigger we know the
        // surface is unresponsive and raise the flag
        child.surface.ping();
        pingPongTimer.running = true;
    }
}
