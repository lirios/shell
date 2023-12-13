// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import Fluid.Controls as FluidControls
import Liri.Shell.Utils as Utils

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

    model: Utils.SortFilterProxyModel {
        id: appsProxyModel

        sourceModel: Utils.SortFilterProxyModel {
            id: appsCategoryProxyModel

            sourceModel: appsModel
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

        AppsGridDelegate {
            id: delegate

            anchors {
                fill: parent
                margins: FluidControls.Units.smallSpacing
            }
        }
    }
}
