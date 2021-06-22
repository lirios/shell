// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Liri.WaylandServer 1.0 as WS

Item {
    id: moveItem

    property bool moving: false

    onMovingChanged: {
        // TODO: Set move cursor when `moving` is true
    }

    ParallelAnimation {
        id: moveAnimation

        SmoothedAnimation { id: xAnimator; target: moveItem; property: "x"; easing.type: Easing.InOutQuad; duration: 450 }
        SmoothedAnimation { id: yAnimator; target: moveItem; property: "y"; easing.type: Easing.InOutQuad; duration: 450 }
    }

    function animateTo(dx, dy) {
        xAnimator.to = dx;
        yAnimator.to = dy;
        moveAnimation.start();
    }
}
