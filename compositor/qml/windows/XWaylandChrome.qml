/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
import QtWayland.Compositor 1.0
import Liri.WaylandServer 1.0
import Liri.XWayland 1.0
import Fluid.Material 1.0

ChromeItem {
    id: chrome

    readonly property alias primary: __private.primary
    property bool minimized: false
    property bool decorated: true

    property alias shellSurface: shellSurfaceItem.shellSurface
    property alias moveItem: shellSurfaceItem.moveItem
    property alias inputEventsEnabled: shellSurfaceItem.inputEventsEnabled
    readonly property alias output: shellSurfaceItem.output

    property rect taskIconGeometry: Qt.rect(0, 0, 32, 32)

    x: shellSurfaceItem.moveItem.x - shellSurfaceItem.output.position.x
    y: shellSurfaceItem.moveItem.y - shellSurfaceItem.output.position.y
    width: shellSurfaceItem.width
    height: shellSurfaceItem.height

    onXChanged: __private.updatePrimary()
    onYChanged: __private.updatePrimary()

    onMinimizedChanged: {
        if (minimized)
            minimizeAnimation.start();
        else
            unminimizeAnimation.start();
    }

    transform: [
        Scale {
            id: scaleTransform
            origin.x: shellSurfaceItem.width / 2
            origin.y: shellSurfaceItem.height / 2
        },
        Scale {
            id: scaleTransformPos
            origin.x: shellSurfaceItem.width / 2
            origin.y: chrome.y - shellSurfaceItem.output.position.y - shellSurfaceItem.height
        }
    ]

    QtObject {
        id: __private

        property bool mapped: false
        property bool primary: false

        property bool unresponsive: false

        property point moveItemPosition

        property var parentSurface: null
        property point offset: Qt.point(0, 0)

        function updatePrimary() {
            var x = chrome.x;
            var y = chrome.y;
            var width = chrome.width;
            var height = chrome.height;
            var area = width * height;
            var screenWidth = output.geometry.width;
            var screenHeight = output.geometry.height;
            var x1 = Math.max(0, x);
            var y1 = Math.max(0, y);
            var x2 = Math.min(x + width, screenWidth);
            var y2 = Math.min(y + height, screenHeight);
            var w1 = Math.max(0, x2 - x1);
            var h1 = Math.max(0, y2 - y1);

            if (w1 * h1 * 2 > area) {
                shellSurfaceItem.setPrimary();
                primary = true;
            } else {
                primary = false;
            }
        }

        function setPosition() {
            var parentSurfaceItem = output.viewsBySurface[__private.parentSurface];
            if (parentSurfaceItem === undefined) {
                var size = Qt.size(shellSurfaceItem.width, shellSurfaceItem.height);
                var pos = chrome.randomPosition(compositor.mousePos, size);
                moveItem.x = pos.x;
                moveItem.y = pos.y;
            } else {
                moveItem.x = parentSurfaceItem.moveItem.x + __private.offset.x;
                moveItem.y = parentSurfaceItem.moveItem.y + __private.offset.y;
            }
        }
    }

    Decoration {
        id: decoration

        anchors.fill: parent
    }

    XWaylandShellSurfaceItem {
        id: shellSurfaceItem

        property int windowType: Qt.Window
        readonly property bool hasDropShadow: !decorated && !shellSurface.maximized && !shellSurface.fullscreen

        x: decorated ? decoration.marginSize : 0
        y: decorated ? decoration.titleBarHeight : 0

        // FIXME: Transparent backgrounds will be opaque due to shadows
        layer.enabled: hasDropShadow
        layer.effect: ElevationEffect {
            elevation: shellSurfaceItem.focus ? 24 : 8
        }

        focusOnClick: true
        onFocusChanged: {
            if (focus) {
                chrome.raise();
                focusAnimation.start();
                applicationManager.focusShellSurface(shellSurface);
            }
        }
        /*
        onSurfaceDestroyed: {
            bufferLocked = true;

            switch (shellSurfaceItem.windowType) {
            case Qt.Window:
                topLevelDestroyAnimation.start();
                break;
            case Qt.SubWindow:
                transientDestroyAnimation.start();
                break;
            case Qt.Popup:
                popupDestroyAnimation.start();
                break;
            default:
                chrome.destroy();
                break;
            }
        }
        */

        Component.onCompleted: shellSurfaceItem.windowType = shellSurface.windowType

        /*
         * Surface events
         */

        Connections {
            target: shellSurfaceItem.surface
            onHasContentChanged: {
                // Animate window creationg
                if (shellSurfaceItem.surface.hasContent && !__private.mapped) {
                    switch (shellSurfaceItem.windowType) {
                    case Qt.Window:
                        __private.setPosition();
                        topLevelMapAnimation.start();
                        break;
                    case Qt.SubWindow:
                        __private.setPosition();
                        transientMapAnimation.start();
                        break;
                    case Qt.Popup:
                        popupMapAnimation.start();
                        break;
                    default:
                        break;
                    }

                    __private.mapped = true;
                }
            }
        }

        /*
         * Shell surface events
         */

        /*
        Connections {
            target: shellSurface
            ignoreUnknownSignals: true
            onWindowTypeChanged: shellSurfaceItem.windowType = shellSurface.windowType
            onActivatedChanged: {
                if (shellSurface.activated) {
                    chrome.raise();
                    focusAnimation.start();
                    applicationManager.focusShellSurface(shellSurface);
                }
            }
            onMinimizedChanged: {
                chrome.minimized = shellSurface.minimized;
            }
            onSetDefaultToplevel: {
                __private.parentSurface = null;
                __private.offset.x = 0;
                __private.offset.y = 0;
            }
            onSetTransient: {
                var parentSurfaceItem = null;

                __private.parentSurface = null;
                __private.offset.x = 0;
                __private.offset.y = 0;

                if (typeof(parentSurface) == "undefined" && typeof(relativeToParent) == "undefined") {
                    if (shellSurface.parentSurface) {
                        __private.parentSurface = shellSurface.parentSurface.surface;
                        parentSurfaceItem = output.viewsBySurface[__private.parentSurface];
                        __private.offset.x = (shellSurfaceItem.width - parentSurfaceItem.width) / 2;
                        __private.offset.y = (shellSurfaceItem.height - parentSurfaceItem.height) / 2;
                    }
                } else {
                    parentSurfaceItem = output.viewsBySurface[parentSurface];
                    __private.parentSurface = parentSurface;
                    __private.offset.x = relativeToParent.x;
                    __private.offset.y = relativeToParent.y;
                }
            }
            onSetPopup: {
                __private.parentSurface = null;
                __private.offset.x = 0;
                __private.offset.y = 0;
            }
            onShowWindowMenu: chrome.showWindowMenu(localSurfacePosition.x, localSurfacePosition.y)
            onDestroyed: output.compositor.handleShellSurfaceDestroyed(shellSurface)
        }
        */

        /*
         * Animations
         */

        Behavior on width {
            SmoothedAnimation { easing.type: Easing.InOutQuad; duration: 350 }
        }

        Behavior on height {
            SmoothedAnimation { easing.type: Easing.InOutQuad; duration: 350 }
        }
    }

    ChromeMenu {
        id: chromeMenu
    }

    /*
     * Animations for top level windows
     */

    SequentialAnimation {
        id: topLevelMapAnimation

        ParallelAnimation {
            NumberAnimation { target: chrome; property: "opacity"; easing.type: Easing.OutExpo; from: 0.0; to: 1.0; duration: 350 }
            NumberAnimation { target: scaleTransform; property: "xScale"; easing.type: Easing.OutExpo; from: 0.01; to: 1.0; duration: 350 }
            NumberAnimation { target: scaleTransform; property: "yScale"; easing.type: Easing.OutExpo; from: 0.1; to: 1.0; duration: 350 }
        }
    }

    SequentialAnimation {
        id: topLevelDestroyAnimation

        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "yScale"; to: 2 / chrome.height; duration: 150 }
            NumberAnimation { target: scaleTransform; property: "xScale"; to: 0.4; duration: 150 }
        }

        NumberAnimation { target: scaleTransform; property: "xScale"; to: 0; duration: 150 }
        NumberAnimation { target: chrome; property: "opacity"; easing.type: Easing.OutQuad; to: 0.0; duration: 200 }

        ScriptAction { script: chrome.destroy() }
    }

    /*
     * Animations for transient windows
     */

    ParallelAnimation {
        id: transientMapAnimation

        NumberAnimation { target: chrome; property: "opacity"; easing.type: Easing.OutQuad; from: 0.0; to: 1.0; duration: 250 }
        NumberAnimation { target: scaleTransformPos; property: "xScale"; easing.type: Easing.OutExpo; from: 0.0; to: 1.0; duration: 250 }
        NumberAnimation { target: scaleTransformPos; property: "yScale"; easing.type: Easing.OutExpo; from: 0.0; to: 1.0; duration: 250 }
    }

    SequentialAnimation {
        id: transientDestroyAnimation

        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "xScale"; easing.type: Easing.OutQuad; from: 1.0; to: 0.0; duration: 200 }
            NumberAnimation { target: scaleTransform; property: "yScale"; easing.type: Easing.OutQuad; from: 1.0; to: 0.0; duration: 200 }
            NumberAnimation { target: chrome; property: "opacity"; easing.type: Easing.OutQuad; from: 1.0; to: 0.0; duration: 200 }
        }

        ScriptAction { script: chrome.destroy() }
    }

    /*
     * Animations for popup windows
     */

    ParallelAnimation {
        id: popupMapAnimation

        NumberAnimation { target: chrome; property: "opacity"; easing.type: Easing.OutQuad; from: 0.0; to: 1.0; duration: 150 }
        NumberAnimation { target: scaleTransform; property: "xScale"; easing.type: Easing.OutExpo; from: 0.9; to: 1.0; duration: 150 }
        NumberAnimation { target: scaleTransform; property: "yScale"; easing.type: Easing.OutExpo; from: 0.9; to: 1.0; duration: 150 }
    }

    ParallelAnimation {
        id: popupDestroyAnimation

        NumberAnimation { target: scaleTransform; property: "xScale"; easing.type: Easing.OutExpo; from: 1.0; to: 0.8; duration: 150 }
        NumberAnimation { target: scaleTransform; property: "yScale"; easing.type: Easing.OutExpo; from: 1.0; to: 0.8; duration: 150 }
        NumberAnimation { target: chrome; property: "opacity"; easing.type: Easing.OutQuad; to: 0.0; duration: 150 }

        ScriptAction { script: chrome.destroy() }
    }

    /*
     * Animation when the window is focused
     */

    SequentialAnimation {
        id: focusAnimation

        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "xScale"; easing.type: Easing.OutQuad; to: 1.02; duration: 100 }
            NumberAnimation { target: scaleTransform; property: "yScale"; easing.type: Easing.OutQuad; to: 1.02; duration: 100 }
        }

        ParallelAnimation {
            NumberAnimation { target: scaleTransform; property: "xScale"; easing.type: Easing.InOutQuad; to: 1.0; duration: 100 }
            NumberAnimation { target: scaleTransform; property: "yScale"; easing.type: Easing.InOutQuad; to: 1.0; duration: 100 }
        }
    }

    /*
     * Animations when the window is minimized or unminimized
     */

    SequentialAnimation {
        id: minimizeAnimation

        ScriptAction {
            script: {
                __private.moveItemPosition.x = shellSurfaceItem.moveItem.x;
                __private.moveItemPosition.y = shellSurfaceItem.moveItem.y;
                moveItem.animateTo(taskIconGeometry.x, taskIconGeometry.y);
            }
        }

        ParallelAnimation {
            NumberAnimation { target: chrome; property: "scale"; easing.type: Easing.OutQuad; to: 0.0; duration: 550 }
            NumberAnimation { target: chrome; property: "opacity"; easing.type: Easing.Linear; to: 0.0; duration: 500 }
        }
    }

    SequentialAnimation {
        id: unminimizeAnimation

        ScriptAction {
            script: {
                moveItem.animateTo(__private.moveItemPosition.x, __private.moveItemPosition.y);
            }
        }

        ParallelAnimation {
            NumberAnimation { target: chrome; property: "scale"; easing.type: Easing.OutQuad; to: 1.0; duration: 550 }
            NumberAnimation { target: chrome; property: "opacity"; easing.type: Easing.Linear; to: 1.0; duration: 500 }
        }
    }

    /*
     * Methods
     */

    function activate() {
        chrome.minimized = false;
        shellSurfaceItem.takeFocus(compositor.defaultSeat);
    }

    function close() {
        // TODO: Close the shell surface with one of the following:
        // shellSurface.sendClose() for xdg_surface
        // shellSurface.sendPopupDone() for xdg_popup
        // shellSurface.surface.destroy() for wl_shell - maybe too aggressive
        console.warn("Close not implemented yet");
    }

    function showWindowMenu(x, y) {
        chromeMenu.x = x;
        chromeMenu.y = y;
        chromeMenu.open();
    }

    function resizeTo(width, height) {
        shellSurfaceItem.sizeFollowsSurface = false;
        shellSurfaceItem.width = width;
        shellSurfaceItem.height = height;
    }

    function restoreSize() {
        shellSurfaceItem.sizeFollowsSurface = true;
        shellSurfaceItem.width = shellSurface.surface.size.width;
        shellSurfaceItem.height = shellSurface.surface.size.height;
    }
}
