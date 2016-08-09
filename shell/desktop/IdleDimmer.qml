/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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

import QtQuick 2.7
import GreenIsland 1.0 as GreenIsland
import Fluid.Core 1.0

Rectangle {
    id: idleDimmer

    color: "black"
    opacity: 0.0

    function fadeIn() {
        if (idleDimmer.opacity == 1.0)
            return

        idleDimmerAnimator.from = 0.0
        idleDimmerAnimator.to = 1.0
        idleDimmerAnimator.start()
    }

    function fadeOut() {
        if (idleDimmer.opacity == 0.0)
            return;

        // Use a timer to compensate for power on time
        idleDimmerTimer.start()
    }

    onOpacityChanged: {
        if (opacity == 1.0)
            output.powerState = GreenIsland.ExtendedOutput.PowerStateStandby
    }

    OpacityAnimator {
        id: idleDimmerAnimator
        target: idleDimmer
        easing.type: Easing.OutSine
        duration: Units.longDuration
    }

    Timer {
        id: idleDimmerTimer
        interval: 1000
        onTriggered: {
            idleDimmerAnimator.from = 1.0
            idleDimmerAnimator.to = 0.0
            idleDimmerAnimator.start()
        }
    }
}
