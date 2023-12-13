// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick

Item {
    id: moveItem

    property bool moving: false

    ParallelAnimation {
        id: moveAnimation

        alwaysRunToEnd: true

        SmoothedAnimation {
            id: xAnimator

            target: moveItem
            property: "x"
            duration: 250
        }

        SmoothedAnimation {
            id: yAnimator

            target: moveItem
            property: "y"
            duration: 250
        }
    }

    function animateTo(dx, dy) {
        xAnimator.to = dx;
        yAnimator.to = dy;
        moveAnimation.start();
    }
}
