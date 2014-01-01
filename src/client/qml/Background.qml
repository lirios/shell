/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Shell 1.0
import Hawaii.Shell.Settings 1.0

Element {
    id: background

    property var settings: BackgroundSettings {
        onTypeChanged: {
            __priv.swapBackgrounds();
        }
        onWallpaperUrlChanged: {
            if (__priv.type == BackgroundSettings.WallpaperBackground)
                wallpaper.source = settings.wallpaperUrl;
        }
    }

    QtObject {
        id: __priv

        property int type
        property bool swapping: false
        property int fadeDuration: 400

        function swapBackgrounds() {
            __priv.type = settings.type;

            switch (settings.type) {
            case BackgroundSettings.ColorBackground:
                switch (settings.colorShading) {
                case BackgroundSettings.SolidColorShading:
                    solid.visible = true;
                    vgradient.visible = false;
                    hgradient.visible = false;
                    break;
                case BackgroundSettings.VerticalColorShading:
                    solid.visible = false;
                    vgradient.visible = true;
                    hgradient.visible = false;
                    break;
                case BackgroundSettings.HorizontalColorShading:
                    solid.visivble = false;
                    vgradient.visible = false;
                    hgradient.visible = true;
                    break;
                default:
                    break;
                }

                wallpaperContainer.visible = false;
                wallpaper.source = "";
                break;
            case BackgroundSettings.WallpaperBackground:
                __priv.swapping = true;
                wallpaper.source = settings.wallpaperUrl;
                break;
            }
        }

        function convertFillMode(value) {
            switch (value) {
            case BackgroundSettings.Scaled:
                return Image.PreserveAspectFit;
            case BackgroundSettings.Cropped:
                return Image.PreserveAspectCrop;
            case BackgroundSettings.Centered:
                return Image.Pad;
            case BackgroundSettings.Tiled:
                return Image.Tile;
            default:
                return Image.Stretch;
            }
        }
    }

    FluidUi.NoiseBackground {
        id: solid
        anchors.fill: parent
        color: settings.primaryColor
        visible: false

        Behavior on color {
            ColorAnimation {
                duration: __priv.fadeDuration
                easing.type: Easing.OutQuad
            }
        }
    }

    FluidUi.NoiseBackground {
        id: vgradient
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: settings.primaryColor

                Behavior on color {
                    ColorAnimation {
                        duration: __priv.fadeDuration
                        easing.type: Easing.OutQuad
                    }
                }
            }
            GradientStop {
                position: 1.0
                color: settings.secondaryColor

                Behavior on color {
                    ColorAnimation {
                        duration: __priv.fadeDuration
                        easing.type: Easing.OutQuad
                    }
                }
            }
        }
        visible: false
    }

    FluidUi.NoiseBackground {
        id: hgradient
        anchors.centerIn: parent
        width: parent.height
        height: parent.width
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: settings.primaryColor

                Behavior on color {
                    ColorAnimation {
                        duration: __priv.fadeDuration
                        easing.type: Easing.OutQuad
                    }
                }
            }
            GradientStop {
                position: 1.0
                color: settings.secondaryColor

                Behavior on color {
                    ColorAnimation {
                        duration: __priv.fadeDuration
                        easing.type: Easing.OutQuad
                    }
                }
            }
        }
        rotation: 270
        visible: false
    }

    FluidUi.NoiseBackground {
        id: wallpaperContainer
        anchors.fill: parent
        color: settings.primaryColor
        visible: false

        FluidUi.SmoothFadeImage {
            property real aspectRatio: background.width / background.height
            id: wallpaper
            anchors.fill: parent
            sourceSize.width: aspectRatio * 1024
            sourceSize.height: 1024
            smooth: true
            clip: wallpaper.fillMode === Image.PreserveAspectCrop
            fillMode: __priv.convertFillMode(settings.fillMode)
            onSourceChanged: {
                if (__priv.swapping) {
                    solid.visible = false;
                    vgradient.visible = false;
                    hgradient.visible = false;
                    wallpaperContainer.visible = true;
                    __priv.swapping = false;
                }
            }
        }
    }

    /*
    Loader {
        id: animatedWallpaper
        anchors.fill: parent
        asynchronous: true
        z: 3
        visible: settings.type === BackgroundSettings.AnimatedWallpaperBackground &&
                 animatedWallpaper.status == Loader.Ready
    }
    */

    Component.onCompleted: {
        __priv.swapBackgrounds();
    }
}
