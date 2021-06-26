// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import Fluid.Controls 1.0 as FluidControls
import Liri.Launcher 1.0 as CppLauncher

ListView {
    signal selected(string category)

    id: root
    currentIndex: 0
    model: CppLauncher.CategoriesModel {
        id: categoriesModel
    }
    delegate: FluidControls.ListItem {
        property string category: model.category

        text: model.display
        highlighted: root.currentIndex == index
        onClicked: root.currentIndex = index
    }
    onCurrentIndexChanged: if (currentItem) root.selected(currentItem.category)

    ScrollBar.vertical: ScrollBar {}
}
