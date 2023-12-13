// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import Fluid.Controls as FluidControls
import Liri.Shell.TaskManager as TaskManager

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
