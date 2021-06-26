// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
