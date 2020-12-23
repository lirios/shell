/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

import QtQuick 2.15
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects

HardwareLayerSurfaceItem {
    id: osd

    layer.enabled: true
    layer.effect: FluidEffects.Elevation {
        elevation: 8
    }

    stackingLevel: 0

    opacity: 0.0
    visible: opacity > 0.0

    OpacityAnimator {
        id: showAnimation

        target: osd
        alwaysRunToEnd: true
        easing.type: Easing.Bezier
        easing.bezierCurve: [0.4, 0.0, 0.2, 1.0]
        duration: FluidControls.Units.mediumDuration
        from: 0.0
        to: 1.0

        onFinished: {
            timer.restart();
        }
    }

    OpacityAnimator {
        id: hideAnimation

        target: osd
        alwaysRunToEnd: true
        easing.type: Easing.Bezier
        easing.bezierCurve: [0.4, 0.0, 0.2, 1.0]
        duration: FluidControls.Units.mediumDuration
        from: 1.0
        to: 0.0
    }

    Timer {
        id: timer

        interval: 2500
        onTriggered: {
            hideAnimation.start();
        }
    }

    Connections {
        target: osdServer

        function onTextRequested(iconName, text) {
            showAnimation.start();
        }

        function onProgressRequested(iconName, value) {
            showAnimation.start();
        }
    }
}
