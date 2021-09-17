// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

Rectangle {
    id: flash

    anchors.fill: parent

    color: "white"
    opacity: 0.0

    SequentialAnimation {
        id: flashAnimation

        OpacityAnimator {
            easing.type: Easing.OutQuad
            target: flash
            from: 0.0
            to: 1.0
            duration: 250
        }
        OpacityAnimator {
            easing.type: Easing.OutQuad
            target: flash
            from: 1.0
            to: 0.0
            duration: 250
        }
    }

    function flash() {
        flashAnimation.start();
    }
}
