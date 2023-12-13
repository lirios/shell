// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Fluid.Controls as FluidControls
import Liri.Shell.Compositor as LS

LS.HotSpot {
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
