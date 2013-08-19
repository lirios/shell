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
import Hawaii.Shell.Desktop 0.1

Item {
    property var settings: BackgroundSettings {}

    Rectangle {
        id: solid
        anchors.fill: parent
        color: settings.primaryColor
        opacity: settings.type === BackgroundSettings.ColorBackground ? 1.0 : 00

        Behavior on opacity {
            NumberAnimation { easing.type: Easing.InQuad; duration: 500 }
        }
    }

    Rectangle {
        id: gradient
        anchors.fill: parent
        opacity: settings.type === BackgroundSettings.ColorBackground && settings.colorShading !== BackgroundSettings.SolidColorShading ? 1.0 : 0.0

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

        Behavior on opacity {
            NumberAnimation { easing.type: Easing.InQuad; duration: 500 }
        }
    }

    Image {
        id: wallpaper
        anchors.fill: parent
        source: settings.wallpaperUrl
        smooth: true
        opacity: settings.type === BackgroundSettings.WallpaperBackground

        Behavior on opacity {
            NumberAnimation { easing.type: Easing.InQuad; duration: 500 }
        }
    }
}
