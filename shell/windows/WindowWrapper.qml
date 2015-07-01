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
import QtCompositor 1.0
import GreenIsland 1.0

Item {
    property var clientWindow
    property var child
    property bool unresponsive: false
    property var animation: null
    property real additionalWidth: 0
    property real additionalHeight: 0

    id: window
    objectName: "clientWindow"
    states: [
        State {
            name: "focused"
            when: child.focus && !window.unresponsive
            PropertyChanges { target: unresponsiveEffectLoader; source: "" }
        },
        State {
            name: "unfocused"
            when: !child.focus && !window.unresponsive
            PropertyChanges { target: unresponsiveEffectLoader; source: "" }
        },
        State {
            name: "focused-unresponsive"
            when: child.focus && window.unresponsive
            PropertyChanges { target: unresponsiveEffectLoader; source: "overlays/UnresponsiveOverlay.qml" }
        },
        State {
            name: "unfocused-unresponsive"
            when: !child.focus && window.unresponsive
            PropertyChanges { target: unresponsiveEffectLoader; source: "overlays/UnresponsiveOverlay.qml" }
        }
    ]
    rotation: {
        switch (_greenisland_output.transform) {
        case WaylandOutput.Transform90:
            return 90;
        case WaylandOutput.TransformFlipped90:
            return -90;
        case WaylandOutput.Transform180:
            return 180;
        case WaylandOutput.TransformFlipped180:
            return -180;
        case WaylandOutput.Transform270:
            return 270;
        case WaylandOutput.TransformFlipped270:
            return -270;
        default:
            break;
        }

        return 0;
    }
    visible: false

    QtObject {
        id: savedData

        property Item parent
        property bool saved: false
    }

    /*
     * Connections to client window, child or surface
     */

    Connections {
        target: child
        onLocalXChanged: window.x = child.localX
        onLocalYChanged: window.y = child.localY
    }

    Connections {
        target: clientWindow
        onFullScreenChanged: {
            // Save old parent and reparent to the full screen layer
            if (clientWindow.fullScreen) {
                savedData.parent = window.parent;
                savedData.saved = true;
                window.parent = compositorRoot.screenView.layers.fullScreen;
                window.anchors.centerIn = window.parent;
            } else {
                window.parent = savedData.parent;
                window.anchors.centerIn = undefined;
                savedData.saved = false;
            }
        }
        onSizeChanged: {
            window.width = clientWindow.size.width + additionalWidth;
            window.height = clientWindow.size.height + additionalHeight;
        }
    }

    Connections {
        target: child ? child.surface : null
        onPong: pongSurface()
    }

    Binding {
        target: child ? child.surface : null
        property: "clientRenderingEnabled"
        value: window.visible
    }

    /*
     * Animations
     */

    Connections {
        target: animation
        onUnmapAnimationStopped: {
            // Destroy window representation
            window.destroy();
        }
        onDestroyAnimationStopped: {
            // Destroy window representation
            window.destroy();
        }
    }

    function runMapAnimation() {
        if (animation && animation.mapAnimation)
            animation.mapAnimation.start();
    }

    function runMinimizeAnimation() {
        if (animation && animation.minimizeAnimation)
            animation.minimizeAnimation.start();
    }

    function runUnminimizeAnimation() {
        if (animation && animation.unminimizeAnimation)
            animation.unminimizeAnimation.start();
    }

    function runUnmapAnimation() {
        if (animation && animation.unmapAnimation)
            animation.unmapAnimation.start();
        else
            window.destroy();
    }

    function runDestroyAnimation() {
        if (animation && animation.destroyAnimation)
            animation.destroyAnimation.start();
        else
            window.destroy();
    }

    /*
     * Ping/pong
     */

    Loader {
        id: unresponsiveEffectLoader
        anchors.fill: parent
        z: visible ? 2 : 0
        visible: status == Loader.Ready
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

    function pongSurface() {
        // Surface replied with a pong this means it's responsive
        pingPongTimer.running = false;
        unresponsive = false;
    }
}
