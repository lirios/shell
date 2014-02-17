/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Configuration 1.0

FluidUi.NoiseBackground {
    id: background

    Behavior on color {
        ColorAnimation {
            duration: 400
            easing.type: Easing.OutQuad
        }
    }

    Configuration {
        id: settings
        category: "shell/backgrounds/org.hawaii.backgrounds.wallpaper"

        property color color: "#336699"
        property url wallpaperUrl: "file:///opt/hawaii/share/backgrounds/hawaii/Also_Calm.png"
        property int fillMode: Image.Stretch

        Component.onCompleted: {
            background.color = settings.color;
            image.source = settings.wallpaperUrl;
            image.fillMode = settings.fillMode;
        }
    }

    FluidUi.SmoothFadeImage {
        property real aspectRatio: parent.width / parent.height

        id: image
        anchors.fill: parent
        sourceSize.width: aspectRatio * 1024
        sourceSize.height: 1024
        smooth: true
        clip: fillMode === Image.PreserveAspectCrop
    }
}
