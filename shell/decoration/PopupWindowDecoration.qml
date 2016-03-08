/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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
import GreenIsland 1.0
import Fluid.Ui 1.0 as FluidUi

Item {
    property alias source: renderer.source
    readonly property real dropShadowExtents: FluidUi.Units.gridUnit
    readonly property real additionalWidth: dropShadowExtents
    readonly property real additionalHeight: dropShadowExtents

    id: root

    BorderImage {
        anchors {
            fill: parent
            margins: -dropShadowExtents
        }
        source: "graphics/dropshadow.sci"
        cache: true
        opacity: root.active ? 0.9 : 0.7
        smooth: true
        z: 0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: FluidUi.Units.shortDuration
            }
        }
    }

    SurfaceRenderer {
        id: renderer
        anchors {
            left: parent.left
            top: parent.top
        }
        width: source.surface.size.width
        height: source.surface.size.height
        z: 1
    }
}
