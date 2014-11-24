/****************************************************************************
 * This file is part of Kahai.
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
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import "."

Item {
    default property alias contents: background.children
    property int edge: Qt.LeftEdge
    readonly property int status: __priv.open ? PlasmaComponents.DialogStatus.Open : PlasmaComponents.DialogStatus.Closed

    id: slidingPanel
    width: units.gridUnit * 10
    height: shellRoot.height
    clip: true
    onEdgeChanged: __priv.resetPosition()
    onWidthChanged: __priv.resetPosition()
    onHeightChanged: __priv.resetPosition()

    Behavior on x {
        NumberAnimation {
            duration: units.longDuration
            easing.type: Easing.OutCubic
        }
    }

    Behavior on y {
        NumberAnimation {
            duration: units.longDuration
            easing.type: Easing.OutCubic
        }
    }

    QtObject {
        id: __priv

        property bool open: false

        function setInitialPosition() {
            switch (edge) {
            case Qt.LeftEdge:
                x = -slidingPanel.width;
                y = 0;
                break;
            case Qt.TopEdge:
                x = 0;
                y = -slidingPanel.height;
                break;
            case Qt.RightEdge:
                x = shellRoot.width + slidingPanel.width;
                y = 0;
                break;
            case Qt.BottomEdge:
                x = 0;
                y = shellRoot.height + slidingPanel.height;
                break;
            default:
                break;
            }
        }

        function setFinalPosition() {
            switch (edge) {
            case Qt.LeftEdge:
                x = 0;
                y = 0;
                break;
            case Qt.TopEdge:
                x = 0;
                y = 0;
                break;
            case Qt.RightEdge:
                x = shellRoot.width - slidingPanel.width;
                y = 0;
                break;
            case Qt.BottomEdge:
                x = 0;
                y = shellRoot.height - slidingPanel.height;
                break;
            default:
                break;
            }
        }

        function resetPosition() {
            if (open)
                setFinalPosition();
            else
                setInitialPosition();
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: Theme.panel.backgroundColor
    }

    Component.onCompleted: __priv.setInitialPosition()

    function open() {
        if (__priv.open)
            return;

        __priv.setFinalPosition();
        __priv.open = true;
    }

    function close() {
        if (!__priv.open)
            return;

        __priv.setInitialPosition();
        __priv.open = false;
    }

    function toggle() {
        if (__priv.open)
            close();
        else
            open();
    }
}
