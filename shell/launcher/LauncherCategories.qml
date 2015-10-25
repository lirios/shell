/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaiios.launcher 0.1 as CppLauncher

ListView {
    signal selected(string category)

    id: root
    onCurrentIndexChanged: if (currentItem) root.selected(currentItem.category)

    Component {
        id: categoryDelegate

        Rectangle {
            property string category: model.category
            property bool hover: false

            color: hover ? Themes.Theme.palette.panel.selectedBackgroundColor : "transparent"
            width: Math.max(ListView.view.width, label.paintedWidth + (2 * Themes.Units.smallSpacing))
            height: Math.max(icon.height, label.paintedHeight) + (2 * Themes.Units.smallSpacing)
            radius: Themes.Units.dp(3)

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: hover = true
                onExited: hover = false
                onClicked: root.currentIndex = index
            }

            Row {
                anchors {
                    fill: parent
                    margins: Themes.Units.smallSpacing
                }
                spacing: Themes.Units.smallSpacing

                Components.Icon {
                    id: icon
                    iconName: model.iconName
                    width: Themes.Units.iconSizes.smallMedium
                    height: width
                }

                Text {
                    id: label
                    anchors.verticalCenter: icon.verticalCenter
                    renderType: Text.NativeRendering
                    text: model.display
                    color: hover ? Themes.Theme.palette.panel.selectedTextColor : Themes.Theme.palette.panel.textColor
                    elide: Text.ElideRight
                    font.bold: root.currentIndex == index
                }
            }
        }
    }

    model: CppLauncher.CategoriesModel {
        id: categoriesModel
    }
    delegate: categoryDelegate
}
