/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Fluid.Ui 1.0
import Hawaii.Shell 1.0
import Hawaii.Shell.Settings 1.0

Item {
    property int type
    property color primaryColor
    property color secondaryColor
    property int colorShading
    property url wallpaperUrl
    property int fillMode

    onWallpaperUrlChanged: {
        // When a renderer is hidden we unload the wallpaper so every time
        // this property is changed we need to set the wallpaper again
        wallpaper.source = wallpaperUrl
    }
    onFillModeChanged: wallpaper.fillMode = convertFillMode(fillMode)

    NoiseBackground {
        id: solid
        anchors.fill: parent
        color: primaryColor
        visible: type == BackgroundSettings.ColorBackground && colorShading == BackgroundSettings.SolidColorShading
    }

    NoiseBackground {
        id: gradient
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: primaryColor
            }
            GradientStop {
                position: 1.0
                color: secondaryColor
            }
        }
        visible: type == BackgroundSettings.ColorBackground && colorShading != BackgroundSettings.SolidColorShading
    }

    Item {
        anchors.fill: parent

        // The fill color rectangle is hidden during the transition
        // between two background renderers otherwise it is visible
        // and that's ugly
        Rectangle {
            id: fillColorRect
            anchors.fill: parent
            color: primaryColor
            visible: type == BackgroundSettings.WallpaperBackground &&
                      ((fillMode == BackgroundSettings.Scaled) ||
                       (fillMode == BackgroundSettings.Centered))
            z: 0
        }

        Image {
            id: wallpaper
            anchors.fill: parent
            sourceSize.width: width
            sourceSize.height: height
            smooth: true
            cache: false
            clip: wallpaper.fillMode == Image.PreserveAspectCrop
            visible: type == BackgroundSettings.WallpaperBackground
            z: 1
        }
    }

    function unloadBackground() {
        wallpaper.source = "";
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
