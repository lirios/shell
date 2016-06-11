/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
import Qt.labs.controls 1.0
import Fluid.Ui 1.0 as FluidUi
import org.hawaiios.launcher 0.1 as CppLauncher

ListView {
    signal selected(string category)

    id: root
    currentIndex: 0
    model: CppLauncher.CategoriesModel {
        id: categoriesModel
    }
    delegate: ItemDelegate {
        property string category: model.category

        width: parent.width
        text: model.display
        highlighted: root.currentIndex == index
        onClicked: root.currentIndex = index
    }
    onCurrentIndexChanged: if (currentItem) root.selected(currentItem.category)

    //ScrollIndicator.vertical: ScrollIndicator {}
}
