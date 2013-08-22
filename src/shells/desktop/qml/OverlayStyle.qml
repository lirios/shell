/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Shell.Styles 0.1

Style {
    property color panelColor1: Qt.rgba(0.13, 0.13, 0.13, 0.7)
    property color panelColor2: Qt.rgba(0, 0, 0, 0.7)
    property color textColor: "white"
    property color textShadowColor: Qt.rgba(0, 0, 0, 60)

    property Component panel: Rectangle {
        id: background
        anchors.fill: parent
        border.color: Qt.rgba(0, 0, 0, 0.5)
        gradient: Gradient {
            GradientStop { position: 0.0; color: panelColor1 }
            GradientStop { position: 0.5; color: panelColor2 }
            GradientStop { position: 1.0; color: panelColor2 }
        }
        radius: 6
        antialiasing: true
    }
}
