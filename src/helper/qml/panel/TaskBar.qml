// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import Fluid.Controls
import Liri.Shell.TaskManager as TaskManager

ListView {
    id: taskBar

    property bool menuOpen: false

    orientation: Qt.Horizontal
    interactive: contentWidth > width

    model: TaskManager.TasksModel {
        id: tasksModel

        sourceModel: appsModel
    }
    delegate: TaskDelegate {}

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
