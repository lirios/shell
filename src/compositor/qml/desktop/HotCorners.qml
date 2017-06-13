/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Liri.WaylandServer 1.0

Item {
    signal topLeftTriggered()
    signal topRightTriggered()

    signal bottomLeftTriggered()
    signal bottomRightTriggered()

    id: root

    // Redirect events
    Connections {
        target: output
        onHotSpotTriggered: {
            switch (hotSpot) {
            case ExtendedOutput.TopLeftHotSpot:
                root.topLeftTriggered();
                break;
            case ExtendedOutput.TopRightHotSpot:
                root.topRightTriggered();
                break;
            case ExtendedOutput.BottomLeftHotSpot:
                root.bottomLeftTriggered();
                break;
            case ExtendedOutput.BottomRightHotSpot:
                root.bottomRightTriggered();
                break;
            default:
                break;
            }
        }
    }

    // Top-left corner
    HotCorner {
        id: topLeftCorner
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: -width / 2
            topMargin: -height / 2
        }
    }

    // Top-right corner
    HotCorner {
        id: topRightCorner
        anchors {
            right: parent.right
            top: parent.top
            topMargin: -height / 2
            rightMargin: -width / 2
        }
    }

    // Bottom-left corner
    HotCorner {
        id: bottomLeftCorner
        anchors {
            left: parent.left
            bottom: parent.bottom
            leftMargin: -width / 2
            bottomMargin: -height / 2
        }
    }

    // Bottom-right corner
    HotCorner {
        id: bottomRightCorner
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: -width / 2
            bottomMargin: -height / 2
        }
    }
}
