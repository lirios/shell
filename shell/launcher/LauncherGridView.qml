/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQml.Models 2.1
import Fluid.Core 1.0
import Fluid.Controls 1.0
import Fluid.Material 1.0
import org.hawaiios.launcher 0.1 as CppLauncher

GridView {
    id: grid

    readonly property int numRows: 6
    readonly property int numColumns: 4
    readonly property int numItemsPerPage: numRows * numColumns
    readonly property int numPages: Math.ceil(grid.count / numItemsPerPage)
    property int currentPage: 0
    property alias query: appsProxyModel.filterPattern

    signal appLaunched()

    cellWidth: 130
    cellHeight: cellWidth
    width: cellWidth * numRows
    height: cellHeight * numColumns
    snapMode: GridView.SnapOneRow
    flow: GridView.LeftToRight
    interactive: true
    clip: true
    preferredHighlightBegin: 0
    preferredHighlightEnd: 0
    highlightRangeMode: GridView.StrictlyEnforceRange
    highlightFollowsCurrentItem: true
    model: SortFilterProxyModel {
        id: appsProxyModel
        sourceModel: CppLauncher.AppsModel {
            id: appsModel
            onAppLaunched: grid.appLaunched()
        }
        filterRoleName: "filterInfo"
        filterCaseSensitivity: Qt.CaseInsensitive
        isSortLocaleAware: true
        sortCaseSensitivity: Qt.CaseSensitive
        sortOrder: Qt.AscendingOrder
        sortRoleName: "name"
    }
    delegate: Item {
        width: grid.cellWidth
        height: grid.cellHeight

        LauncherGridDelegate {
            id: delegate

            anchors {
                fill: parent
                margins: Units.smallSpacing
            }

            Ripple {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                hoverEnabled: true
                onEntered: delegate.hovered = true
                onExited: delegate.hovered = false
                onClicked: appsModel.trigger(appsProxyModel.mapToSource(index))
            }
        }
    }

    function filterByCategory(category) {
        appsModel.categoryFilter = category;
    }
}
