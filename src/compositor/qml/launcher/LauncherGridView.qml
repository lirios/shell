/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQml.Models 2.1
import Fluid.Core 1.0
import Fluid.Controls 1.0
import Liri.Launcher 0.1
import "../components"

PagedGrid {
    id: gridView

    property alias query: appsProxyModel.filterPattern

    property int cellSize: 130

    width: columns * cellSize
    height: rows * cellSize

    rows: 4
    columns: 6

    function filterByCategory(category) {
        appsCategoryProxyModel.filterPattern = category;
    }

    model: SortFilterProxyModel {
        id: appsProxyModel
        sourceModel: SortFilterProxyModel {
            id: appsCategoryProxyModel
            sourceModel: applicationManager
            filterRoleName: "categories"
            filterCaseSensitivity: Qt.CaseInsensitive
        }
        filterRoleName: "filterInfo"
        filterCaseSensitivity: Qt.CaseInsensitive
        isSortLocaleAware: true
        sortCaseSensitivity: Qt.CaseSensitive
        sortOrder: Qt.AscendingOrder
        sortRoleName: "name"
    }

    delegate: Item {

        width: 130
        height: width

        LauncherGridDelegate {
            id: delegate

            anchors {
                fill: parent
                margins: Units.smallSpacing
            }
        }
    }
}
