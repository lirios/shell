// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import Fluid.Controls 1.0 as FluidControls
import Liri.Shell.TaskManager 1.0 as TaskManager

ListView {
    signal selected(string category)

    currentIndex: 0
    model: TaskManager.CategoriesModel {
        id: categoriesModel
    }
    delegate: FluidControls.ListItem {
        property string category: model.category

        text: model.display
        highlighted: currentIndex === index
        onClicked: {
            currentIndex = index;
        }
    }
    onCurrentIndexChanged: {
        if (currentItem)
            selected(currentItem.category);
    }

    ScrollBar.vertical: ScrollBar {}
}
