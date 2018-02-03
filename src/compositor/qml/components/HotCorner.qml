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
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Liri.WaylandServer 1.0
import Fluid.Controls 1.0 as FluidControls

HotSpot {
    anchors {
        left: {
            switch (corner) {
            case Qt.TopLeftCorner:
            case Qt.BottomLeftCorner:
                return parent.left;
            default:
                break;
            }
            return undefined;
        }
        top: {
            switch (corner) {
            case Qt.TopLeftCorner:
            case Qt.TopRightCorner:
                return parent.top;
            default:
                break;
            }
            return undefined;
        }
        right: {
            switch (corner) {
            case Qt.TopRightCorner:
            case Qt.BottomRightCorner:
                return parent.right;
            default:
                break;
            }
            return undefined;
        }
        bottom: {
            switch (corner) {
            case Qt.BottomLeftCorner:
            case Qt.BottomRightCorner:
                return parent.bottom;
            default:
                break;
            }
            return undefined;
        }
    }
    width: 32
    height: width

    Rectangle {
        anchors {
            left: {
                switch (parent.corner) {
                case Qt.TopLeftCorner:
                case Qt.BottomLeftCorner:
                    return parent.left;
                default:
                    break;
                }
                return undefined;
            }
            top: {
                switch (parent.corner) {
                case Qt.TopLeftCorner:
                case Qt.TopRightCorner:
                    return parent.top;
                default:
                    break;
                }
                return undefined;
            }
            right: {
                switch (parent.corner) {
                case Qt.TopRightCorner:
                case Qt.BottomRightCorner:
                    return parent.right;
                default:
                    break;
                }
                return undefined;
            }
            bottom: {
                switch (parent.corner) {
                case Qt.BottomLeftCorner:
                case Qt.BottomRightCorner:
                    return parent.bottom;
                default:
                    break;
                }
                return undefined;
            }
            leftMargin: {
                switch (parent.corner) {
                case Qt.TopLeftCorner:
                case Qt.BottomLeftCorner:
                    return -width / 2;
                default:
                    break;
                }
                return undefined;
            }
            topMargin: {
                switch (parent.corner) {
                case Qt.TopLeftCorner:
                case Qt.TopRightCorner:
                    return -height / 2;
                default:
                    break;
                }
                return undefined;
            }
            rightMargin: {
                switch (parent.corner) {
                case Qt.TopRightCorner:
                case Qt.BottomRightCorner:
                    return -width / 2;
                default:
                    break;
                }
                return undefined;
            }
            bottomMargin: {
                switch (parent.corner) {
                case Qt.BottomLeftCorner:
                case Qt.BottomRightCorner:
                    return -height / 2;
                default:
                    break;
                }
                return undefined;
            }
        }
        width: parent.width
        height: width
        radius: width / 2
        color: Material.accent
        opacity: parent.hovered ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: FluidControls.Units.shortDuration
            }
        }
    }
}
