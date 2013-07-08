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
import FluidCore 0.2 as FluidCore

Item {
    FluidCore.Settings {
        id: settings
        category: "background"

        property string backgroundType: "color"
        property color primaryColor: "#336699"
        property color secondaryColor: "#2e5d8c"
        property string colorShadingType: "solid"
        property url wallpaperUrl

        onBackgroundTypeChanged: {
            switch (backgroundType) {
            case "color":
                switch (colorShadingType) {
                case "solid":
                    solidAnimation.start();
                    break;
                case "horizontal":
                case "vertical":
                    gradientAnimation.start();
                    break;
                }
                break;
            case "wallpaper":
                wallpaperAnimation.start();
                break;
            default:
                blankAnimation.start();
                break;
            }
        }
    }

    Rectangle {
        id: solid
        anchors.fill: parent
        color: settings.primaryColor
        opacity: settings.backgroundType == "color" ? 1.0 : 0.0
    }

    Rectangle {
        id: gradient
        anchors.fill: parent
        opacity: settings.backgroundType == "color" && settings.colorShadingType != "solid" ? 1.0 : 00

        Gradient {
            GradientStop {
                position: 0.0
                color: settings.primaryColor
            }
            GradientStop {
                position: 1.0
                color: settings.secondaryColor
            }
        }
    }

    Image {
        id: wallpaper
        anchors.fill: parent
        source: settings.wallpaperUrl
        smooth: true
        opacity: settings.backgroundType == "wallpaper" ? 1.0 : 0.0
    }

    SequentialAnimation {
        id: solidAnimation
        NumberAnimation { target: gradient; property: "opacity"; to: 0.0; duration: 200 }
        NumberAnimation { target: wallpaper; property: "opacity"; to: 0.0; duration: 200 }
        NumberAnimation { target: solid; property: "opacity"; to: 1.0; duration: 250 }
    }

    SequentialAnimation {
        id: gradientAnimation
        NumberAnimation { target: solid; property: "opacity"; to: 0.0; duration: 200 }
        NumberAnimation { target: wallpaper; property: "opacity"; to: 0.0; duration: 200 }
        NumberAnimation { target: gradient; property: "opacity"; to: 1.0; duration: 250 }
    }

    SequentialAnimation {
        id: wallpaperAnimation
        NumberAnimation { target: solid; property: "opacity"; to: 0.0; duration: 200 }
        NumberAnimation { target: gradient; property: "opacity"; to: 0.0; duration: 200 }
        NumberAnimation { target: wallpaper; property: "opacity"; to: 1.0; duration: 250 }
    }

    SequentialAnimation {
        id: blankAnimation
        NumberAnimation { target: solid; property: "opacity"; to: 0.0; duration: 200 }
        NumberAnimation { target: gradient; property: "opacity"; to: 0.0; duration: 200 }
        NumberAnimation { target: wallpaper; property: "opacity"; to: 0.0; duration: 250 }
    }
}
