/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.3
import Hawaii.Themes 1.0 as Themes
import Fluid.Ui 1.0 as FluidUi

Item {
    default property alias contents: background.children
    property int edge: Qt.LeftEdge
    readonly property alias expanded: __priv.open

    id: slidingPanel
    clip: true
    visible: false

    XAnimator {
        id: xAnimator
        target: slidingPanel
        easing.type: Easing.OutCubic
        duration: FluidUi.Units.longDuration
    }

    YAnimator {
        id: yAnimator
        target: slidingPanel
        easing.type: Easing.OutCubic
        duration: FluidUi.Units.longDuration
    }

    QtObject {
        id: __priv

        property bool open: false

        onOpenChanged: {
            // Calculate implicit size
            if (open) {
                implicitWidth = __priv.calcWidth();
                implicitHeight = __priv.calcHeight();
            }

            // Animate
            switch (edge) {
            case Qt.LeftEdge:
                xAnimator.from = open ? -slidingPanel.width : 0;
                xAnimator.to = open ? 0 : -slidingPanel.width;
                xAnimator.start();
                break;
            case Qt.TopEdge:
                yAnimator.from = open ? -slidingPanel.height : 0;
                yAnimator.to = open ? 0 : -slidingPanel.height;
                yAnimator.start();
                break;
            case Qt.RightEdge:
                xAnimator.from = open ? screenView.width + slidingPanel.width : screenView.width - slidingPanel.width;
                xAnimator.to = open ? screenView.width - slidingPanel.width : screenView.width + slidingPanel.width;
                xAnimator.start();
                break;
            case Qt.BottomEdge:
                yAnimator.from = open ? screenView.height + slidingPanel.height : screenView.height - slidingPanel.height;
                yAnimator.to = open ? screenView.height - slidingPanel.height : screenView.height + slidingPanel.height;
                yAnimator.start();
                break;
            default:
                break;
            }
        }

        function calcWidth() {
            if (slidingPanel.edge == Qt.TopEdge || slidingPanel.edge == Qt.BottomEdge)
                return output.availableGeometry.width;
            return Math.max(FluidUi.Units.gu(10), slidingPanel.width, slidingPanel.childrenRect.width);
        }

        function calcHeight() {
            if (slidingPanel.edge == Qt.LeftEdge || slidingPanel.edge == Qt.RightEdge)
                return output.availableGeometry.height;
            return Math.max(FluidUi.Units.gu(10), slidingPanel.height, slidingPanel.childrenRect.height);
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: Themes.Theme.palette.panel.backgroundColor
    }

    function open() {
        if (__priv.open)
            return;

        visible = true;
        __priv.open = true;
    }

    function close() {
        if (!__priv.open)
            return;

        __priv.open = false;
    }

    function toggle() {
        if (__priv.open)
            close();
        else
            open();
    }
}
