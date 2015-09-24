/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import ".."

Loader {
    Connections {
        target: ShellSettings.background
        onModeChanged: readSettings()
    }

    Component {
        id: solid

        Components.NoiseBackground {
            objectName: "solid"
            color: ShellSettings.background.primaryColor

            Behavior on color {
                ColorAnimation {
                    easing.type: Easing.OutQuad
                    duration: Themes.Units.mediumDuration
                }
            }
        }
    }

    Component {
        id: gradient

        Components.NoiseBackground {
            property bool vertical: ShellSettings.background.mode === "vgradient"

            objectName: "gradient"
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: ShellSettings.background.primaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: Themes.Units.mediumDuration
                        }
                    }
                }
                GradientStop {
                    position: 1
                    color: ShellSettings.background.secondaryColor

                    Behavior on color {
                        ColorAnimation {
                            easing.type: Easing.OutQuad
                            duration: Themes.Units.mediumDuration
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

        Components.NoiseBackground {
            objectName: "wallpaper"
            color: ShellSettings.background.primaryColor

            Components.SmoothFadeImage {
                readonly property real aspectRatio: width / height

                anchors.fill: parent
                source: ShellSettings.background.pictureUrl
                sourceSize.width: aspectRatio * 1024
                sourceSize.height: 1024
                smooth: true
                clip: fillMode === Image.PreserveAspectCrop
                fillMode: ShellSettings.convertFillMode(ShellSettings.background.fillMode)
            }
        }
    }

    function readSettings() {
        switch (ShellSettings.background.mode) {
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
