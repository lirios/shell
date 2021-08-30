// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Liri.Shell.TaskManager 1.0 as TaskManager

Item {
    id: pagedGrid

    property var model

    property int rows
    property int columns

    property Component delegate

    readonly property var count: model.count

    readonly property int pageCount: rows * columns
    readonly property alias pages: pageView.count
    property alias currentPage: pageView.currentIndex

    // Scroll through pages using the mouse weel
    WheelHandler {
        acceptedButtons: Qt.NoButton
        onWheel: {
            if (event.hasAngleDelta) {
                if (event.angleDelta.y > 0)
                    pageView.decrementCurrentIndex();
                else
                    pageView.incrementCurrentIndex();
            }
        }
    }

    ListView {
        id: pageView

        anchors.fill: parent

        orientation: Qt.Horizontal
        snapMode: ListView.SnapOneItem
        highlightFollowsCurrentItem: true
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightMoveDuration: 500
        currentIndex: 0
        clip: true

        model: Math.ceil(pagedGrid.count/pageCount)

        delegate: Grid {
            id: page

            readonly property int pageIndex: index

            width: pagedGrid.width
            height: pagedGrid.height

            columns: pagedGrid.columns

            Repeater {
                model: TaskManager.PageModel {
                    id: pageModel

                    sourceModel: pagedGrid.model
                    startIndex: pageCount * pageIndex
                    limitCount: pageCount
                }

                delegate: pagedGrid.delegate
            }
        }
    }
}
