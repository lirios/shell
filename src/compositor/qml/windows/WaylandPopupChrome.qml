/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.15
import QtWayland.Compositor 1.15

Item {
    id: chrome

    property QtObject window

    anchors.left: parent.shellSurfaceItem ? parent.shellSurfaceItem.left : undefined
    anchors.top: parent.shellSurfaceItem ? parent.shellSurfaceItem.top : undefined
    anchors.leftMargin: (window ? window.offset.x : 0)
    anchors.topMargin: (window ? window.offset.y : 0)

    implicitWidth: shellSurfaceItem.width
    implicitHeight: shellSurfaceItem.height

    ShellSurfaceItem {
        id: shellSurfaceItem

        shellSurface: chrome.window.shellSurface

        inputEventsEnabled: !output.screenView.locked

        focusOnClick: false

        onSurfaceDestroyed: {
            bufferLocked = true;
            destroyAnimation.start();
        }
    }

    /*
     * Animations
     */

    SequentialAnimation {
        id: destroyAnimation

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
}
