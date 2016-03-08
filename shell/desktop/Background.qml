/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

import QtQuick 2.0
import Fluid.Ui 1.0 as FluidUi
import ".."

Loader {
    Connections {
        target: hawaiiCompositor.settings.background
        onModeChanged: readSettings()
    }

    Component {
        id: solid

        FluidUi.NoiseBackground {
            objectName: "solid"
            color: hawaiiCompositor.settings.background.primaryColor

            Behavior on color {
                ColorAnimation {
                    easing.type: Easing.OutQuad
                    duration: FluidUi.Units.mediumDuration
                }
            }
        }
    }

    Component {
        id: gradient

        FluidUi.NoiseBackground {
            property bool vertical: hawaiiCompositor.settings.background.mode === "vgradient"

            objectName: "gradient"
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: hawaiiCompositor.settings.background.primaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: FluidUi.Units.mediumDuration
                        }
                    }
                }
                GradientStop {
                    position: 1
                    color: hawaiiCompositor.settings.background.secondaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: FluidUi.Units.mediumDuration
                        }
                    }
                }
            }
            rotation: vertical ? 270 : 0
            scale: vertical ? 2 : 1
        }
    }

    Component {
        id: wallpaper

        FluidUi.NoiseBackground {
            objectName: "wallpaper"
            color: hawaiiCompositor.settings.background.primaryColor

            FluidUi.SmoothFadeImage {
                readonly property real aspectRatio: width / height

                anchors.fill: parent
                source: hawaiiCompositor.settings.background.pictureUrl
                sourceSize.width: aspectRatio * 1024
                sourceSize.height: 1024
                smooth: true
                clip: fillMode === Image.PreserveAspectCrop
                fillMode: hawaiiCompositor.settings.convertFillMode(hawaiiCompositor.settings.background.fillMode)
            }
        }
    }

    function readSettings() {
        switch (hawaiiCompositor.settings.background.mode) {
        case "solid":
            sourceComponent = solid;
            break;
        case "hgradient":
        case "vgradient":
            sourceComponent = gradient;
            break;
        case "wallpaper":
            sourceComponent = wallpaper;
            break;
        default:
            sourceComponent = null;
            break;
        }
    }

    Component.onCompleted: readSettings()
}
