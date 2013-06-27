/****************************************************************************
 * This file is part of Desktop Shell.
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
import QtQuick.Window 2.0
import FluidCore 1.0 as FluidCore

Window {
    color: "black"

    FluidCore.Settings {
        id: settings
        schema: "org.hawaii.desktop"
        group: "background"
        onValueChanged: loadSettings()
    }

    Rectangle {
        id: solid
        anchors.fill: parent
        color: settings.value("primary-color")
        opacity: 0.0
    }

    Rectangle {
        id: gradient
        anchors.fill: parent
        opacity: 0.0

        Gradient {
            GradientStop {
                position: 0.0
                color: settings.value("primary-color")
            }
            GradientStop {
                position: 1.0
                color: settings.value("secondary-color")
            }
        }
    }

    Image {
        id: wallpaper
        anchors.fill: parent
        source: settings.value("wallpaper-uri")
        smooth: true
        opacity: 0.0
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

    function loadInitialSettings() {
        var type = settings.value("type");

        if (type === "color") {
            var shadingType = settings.value("color-shading-type");

            if (shadingType === "solid")
                solid.opacity = 1.0;
            else if (shadingType === "horizontal" || shadingType === "vertical")
                gradient.opacity = 1.0;
        } else if (type === "wallpaper")
            wallpaper.opacity = 1.0;
    }

    function loadSettings() {
        var type = settings.value("type");

        if (type === "color") {
            var shadingType = settings.value("color-shading-type");

            if (shadingType === "solid")
                solidAnimation.start();
            else if (shadingType === "horizontal" || shadingType === "vertical")
                gradientAnimation.start();
        } else if (type === "wallpaper")
            wallpaperAnimation.start();
        else
            blankAnimation.start();
    }

    Component.onCompleted: loadInitialSettings()
}
