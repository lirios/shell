/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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
import Fluid.Controls 1.0

Loader {
    sourceComponent: {
        switch (compositor.settings.background.mode) {
        case "solid":
            return solid
        case "hgradient":
        case "vgradient":
            return gradient
        case "wallpaper":
            return wallpaper
        default:
            return null
        }
    }

    Component {
        id: solid

        NoiseBackground {
            objectName: "solid"
            color: compositor.settings.background.primaryColor

            Behavior on color {
                ColorAnimation {
                    easing.type: Easing.OutQuad
                    duration: Units.mediumDuration
                }
            }
        }
    }

    Component {
        id: gradient

        NoiseBackground {
            property bool vertical: compositor.settings.background.mode === "vgradient"

            objectName: "gradient"
            rotation: vertical ? 270 : 0
            scale: vertical ? 2 : 1
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: compositor.settings.background.primaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: Units.mediumDuration
                        }
                    }
                }
                GradientStop {
                    position: 1
                    color: compositor.settings.background.secondaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: Units.mediumDuration
                        }
                    }
                }
            }
        }
    }

    Component {
        id: wallpaper

        NoiseBackground {
            objectName: "wallpaper"
            color: compositor.settings.background.primaryColor

            SmoothFadeImage {
                readonly property real aspectRatio: width / height

                anchors.fill: parent
                source: compositor.settings.background.pictureUrl
                sourceSize.width: aspectRatio * 1024
                sourceSize.height: 1024
                smooth: true
                clip: fillMode === Image.PreserveAspectCrop
                fillMode: compositor.settings.convertFillMode(compositor.settings.background.fillMode)
            }
        }
    }
}
