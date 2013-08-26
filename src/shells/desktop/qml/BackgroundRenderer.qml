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
    property int type
    property color primaryColor
    property color secondaryColor
    property int colorShading
    property url wallpaperUrl

    Rectangle {
        id: solid
        anchors.fill: parent
        color: primaryColor
        visible: type === BackgroundSettings.ColorBackground
    }

    Rectangle {
        id: gradient
        anchors.fill: parent
        visible: type === BackgroundSettings.ColorBackground && colorShading !== BackgroundSettings.SolidColorShading

        Gradient {
            GradientStop {
                position: 0.0
                color: primaryColor
            }
            GradientStop {
                position: 1.0
                color: secondaryColor
            }
        }
    }

    Image {
        id: wallpaper
        anchors.fill: parent
        source: wallpaperUrl
        smooth: true
        visible: type === BackgroundSettings.WallpaperBackground
    }
}
