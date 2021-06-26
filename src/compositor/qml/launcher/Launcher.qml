// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Fluid.Controls 1.0
import Liri.Launcher 1.0

ListView {
    id: launcher

    orientation: Qt.Horizontal
    interactive: contentWidth > width

    model: launcherModel
    delegate: LauncherDelegate {}

    remove: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; to: 0; duration: 250 }
            NumberAnimation { properties: "y"; to: height; duration: 250 }
        }
    }

    removeDisplaced: Transition {
        SequentialAnimation {
            PauseAnimation { duration: 250 }
            NumberAnimation { properties: "x,y"; duration: 250 }
        }
    }

    move: Transition {
        SequentialAnimation {
            PauseAnimation { duration: 250 }
            NumberAnimation { properties: "x,y"; duration: 250 }
        }
    }

    moveDisplaced: Transition {
        SequentialAnimation {
            PauseAnimation { duration: 250 }
            NumberAnimation { properties: "x,y"; duration: 250 }
        }
    }

    add: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; to: 1; from: 0; duration: 250 }
            NumberAnimation { properties: "y"; to: 0; from: height; duration: 250 }
        }
    }
}
