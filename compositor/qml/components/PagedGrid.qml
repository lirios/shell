/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
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

import QtQuick 2.0
import Liri.Launcher 0.1

Item {
    id: pagedGrid

    property var model

    property int rows
    property int columns

    property Component delegate

    readonly property var count: model.rowCount()

    readonly property int pageCount: rows * columns
    readonly property alias pages: pageView.count
    property alias currentPage: pageView.currentIndex

    // Scroll through pages using the mouse weel
    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: true

        onWheel: {
            if (wheel.angleDelta.y > 0)
                pageView.decrementCurrentIndex();
            else
                pageView.incrementCurrentIndex();
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
                model: PageModel {
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
