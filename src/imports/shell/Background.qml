/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
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
import QtGraphicalEffects 1.0
import Fluid.Core 1.0
import Fluid.Controls 1.0 as FluidControls

Loader {
    id: background

    property string mode
    property url pictureUrl
    property string primaryColor
    property string secondaryColor
    property string fillMode
    property bool blur: false
    property real blurRadius: 32
    readonly property bool loaded: __private.loaded

    sourceComponent: {
        switch (background.mode) {
        case "solid":
            return solid
        case "hgradient":
        case "vgradient":
            return gradient
        case "wallpaper":
            return wallpaper
        default:
            break
        }
        return null
    }

    QtObject {
        id: __private

        property bool loaded: false
    }

    Component {
        id: solid

        Rectangle {
            objectName: "solid"
            color: background.primaryColor

            Component.onCompleted: {
                __private.loaded = true;
            }

            Behavior on color {
                ColorAnimation {
                    easing.type: Easing.OutQuad
                    duration: FluidControls.Units.mediumDuration
                }
            }
        }
    }

    Component {
        id: gradient

        Rectangle {
            property bool vertical: background.mode === "vgradient"

            objectName: "gradient"
            rotation: vertical ? 270 : 0
            scale: vertical ? 2 : 1
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: background.primaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: FluidControls.Units.mediumDuration
                        }
                    }
                }
                GradientStop {
                    position: 1
                    color: background.secondaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: FluidControls.Units.mediumDuration
                        }
                    }
                }
            }

            Component.onCompleted: {
                __private.loaded = true;
            }
        }
    }

    Component {
        id: wallpaper

        Item {
            objectName: "wallpaper"

            Rectangle {
                anchors.fill: parent
                color: background.primaryColor
                visible: picture.status !== Image.Loading

                Behavior on color {
                    ColorAnimation {
                        easing.type: Easing.OutQuad
                        duration: FluidControls.Units.mediumDuration
                    }
                }
            }

            FluidControls.SmoothFadeImage {
                readonly property real aspectRatio: width / height

                id: picture
                anchors.fill: parent
                source: background.pictureUrl
                sourceSize.width: width
                sourceSize.height: height
                fillMode: background.convertFillMode(background.fillMode)
                onStatusChanged: __private.loaded = picture.status === Image.Ready
                visible: !blur.visible
            }

            FastBlur {
                id: blur
                anchors.fill: parent
                source: picture
                radius: background.blurRadius
                visible: background.blur
            }
        }
    }

    function convertFillMode(fillMode) {
        switch (fillMode) {
        case "preserve-aspect-fit":
            return Image.PreserveAspectFit;
        case "preserve-aspect-crop":
            return Image.PreserveAspectCrop;
        case "tile":
            return Image.Tile;
        case "tile-vertically":
            return Image.TileVertically;
        case "tile-horizontally":
            return Image.TileHorizontally;
        case "pad":
            return Image.Pad;
        default:
            return Image.Stretch;
        }
    }
}
