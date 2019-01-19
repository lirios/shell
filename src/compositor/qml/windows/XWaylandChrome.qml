/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Liri.XWayland 1.0
import Liri.Shell 1.0 as LS
import Liri.private.shell 1.0 as P
import Fluid.Effects 1.0 as FluidEffects

P.ChromeItem {
    id: chrome

    property QtObject window

    property rect taskIconGeometry: Qt.rect(0, 0, 32, 32)

    x: chrome.window.moveItem.x - shellSurfaceItem.output.position.x
    y: chrome.window.moveItem.y - shellSurfaceItem.output.position.y

    implicitWidth: shellSurfaceItem.width + (2 * chrome.window.borderSize)
    implicitHeight: shellSurfaceItem.height + (2 * chrome.window.borderSize) + chrome.window.titleBarHeight

    shellSurfaceItem: shellSurfaceItem

    // FIXME: Transparent backgrounds will be opaque due to shadows
    layer.enabled: chrome.window.mapped && chrome.window.bordered
    layer.effect: FluidEffects.Elevation {
        elevation: shellSurfaceItem.focus ? 24 : 8
    }

    transform: [
        Scale {
            id: scaleTransform
            origin.x: chrome.width / 2
            origin.y: chrome.height / 2
        },
        Scale {
            id: scaleTransformPos
            origin.x: chrome.width / 2
            origin.y: chrome.y - shellSurfaceItem.output.position.y - chrome.height
        }
    ]

    // This is fundamental for scaled windows with the present windows effect,
    // the default value of Item.Center offsets the window position
    transformOrigin: Item.TopLeft

    QtObject {
        id: __private

        property bool unresponsive: false

        property point moveItemPosition

        function setPosition() {
            var parentSurfaceItem = shellSurfaceItem.output.viewsBySurface[chrome.window.parentSurface];
            if (parentSurfaceItem) {
                if (chrome.window.windowType === Qt.Popup) {
                    chrome.parent = parentSurfaceItem;
                    chrome.x = Qt.binding(function() { return chrome.window.shellSurface.x; });
                    chrome.y = Qt.binding(function() { return chrome.window.shellSurface.y; });
                } else {
                    chrome.window.moveItem.x = parentSurfaceItem.window.moveItem.x + ((parentSurfaceItem.width - chrome.width) / 2);
                    chrome.window.moveItem.y = parentSurfaceItem.window.moveItem.y + ((parentSurfaceItem.height - chrome.height) / 2);
                }
            } else {
                var pos = chrome.randomPosition(liriCompositor.mousePos);
                chrome.window.moveItem.x = pos.x;
                chrome.window.moveItem.y = pos.y;
            }
        }
    }

    Connections {
        target: chrome.window
        ignoreUnknownSignals: true
        onMappedChanged: {
            if (chrome.window.mapped) {
                if (chrome.window.focusable)
                    takeFocus();
                __private.setPosition();
                mapAnimation.start();
            }
        }
        onActivatedChanged: {
            if (chrome.window.activated) {
                chrome.raise();
                focusAnimation.start();
            }
        }
        onMinimizedChanged: {
            if (chrome.window.minimized)
                minimizeAnimation.start();
            else
                unminimizeAnimation.start();
        }
        onShowWindowMenu: {
            showWindowMenu(localSurfacePosition.x, localSurfacePosition.y);
        }
    }

    Decoration {
        id: decoration

        anchors.fill: parent
        drag.target: chrome.window.moveItem
        enabled: chrome.window.decorated
        visible: chrome.window.mapped && enabled
    }

    XWaylandShellSurfaceItem {
        id: shellSurfaceItem

        x: chrome.window.borderSize
        y: chrome.window.borderSize + chrome.window.titleBarHeight

        shellSurface: chrome.window.shellSurface
        moveItem: chrome.window.moveItem

        inputEventsEnabled: !output.screenView.locked

        focusOnClick: chrome.window.focusable

        onSurfaceDestroyed: {
            bufferLocked = true;
            if (chrome.window.windowType === Qt.Popup)
                popupDestroyAnimation.start();
            else
                destroyAnimation.start();
        }

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
     * Animations for creation and destruction
     */

    ParallelAnimation {
        id: mapAnimation

        NumberAnimation { target: chrome; property: "scale"; from: 0.9; to: 1.0; easing.type: Easing.OutQuint; duration: 220 }
        NumberAnimation { target: chrome; property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.OutCubic; duration: 150 }
    }

    SequentialAnimation {
        id: destroyAnimation

        ParallelAnimation {
            NumberAnimation { target: chrome; property: "scale"; from: 1.0; to: 0.9; easing.type: Easing.OutQuint; duration: 220 }
            NumberAnimation { target: chrome; property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.OutCubic; duration: 150 }
        }

        ScriptAction {
            script: {
                chrome.destroy();

                if (chrome.primary)
                    liriCompositor.handleShellSurfaceDestroyed(window);
            }
        }
    }

    SequentialAnimation {
        id: popupDestroyAnimation

        ParallelAnimation {
            NumberAnimation { target: chrome; property: "scale"; from: 1.0; to: 0.9; easing.type: Easing.OutQuint; duration: 220 }
            NumberAnimation { target: chrome; property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.OutCubic; duration: 150 }
        }

        ScriptAction {
            script: {
                chrome.destroy();
            }
        }
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

    function showWindowMenu(x, y) {
        chromeMenu.x = x;
        chromeMenu.y = y;
        chromeMenu.open();
    }
}
