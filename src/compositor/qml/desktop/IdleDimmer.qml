// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import Fluid.Controls as FluidControls
import Liri.Shell.Compositor as LS

Rectangle {
    id: idleDimmer

    color: "black"
    opacity: 0.0

    function fadeIn() {
        if (idleDimmer.opacity > 0.0)
            return;

        // Make sure the power on timer is stopped
        idleDimmerTimer.stop();

        idleDimmerAnimator.from = 0.0;
        idleDimmerAnimator.to = 1.0;
        idleDimmerAnimator.start();
    }

    function fadeOut() {
        if (idleDimmer.opacity < 1.0)
            return;

        // Use a timer to compensate for power on time
        idleDimmerTimer.start();
    }

    onOpacityChanged: {
        // Power off output when fade out has finished
        if (opacity == 1.0)
            output.screen.powerState = LS.ScreenItem.PowerStateStandby;
    }

    OpacityAnimator {
        id: idleDimmerAnimator

        target: idleDimmer
        easing.type: Easing.OutSine
        duration: FluidControls.Units.longDuration
    }

    Timer {
        id: idleDimmerTimer

        interval: 1000
        repeat: false
        onTriggered: {
            idleDimmerAnimator.from = 1.0;
            idleDimmerAnimator.to = 0.0;
            idleDimmerAnimator.start();
        }
    }
}
