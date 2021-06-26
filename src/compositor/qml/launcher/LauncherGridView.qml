// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQml.Models 2.1
import Fluid.Core 1.0
import Fluid.Controls 1.0
import Liri.Launcher 1.0
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
