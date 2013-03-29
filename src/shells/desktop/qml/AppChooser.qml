/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import GreenIsland 1.0
import DesktopShell 0.1
import FluidCore 1.0
import FluidUi 1.0

ShellWindow {
    color: "transparent"
    width: 600
    height: 600

    FrameSvgItem {
        id: frame
        anchors.fill: parent
        imagePath: "dialogs/background"
    }

    Item {
        id: mainItem
        anchors {
            fill: frame
            leftMargin: frame.margins.left + 8
            topMargin: frame.margins.top + 8
            rightMargin: frame.margins.right + 8
            bottomMargin: frame.margins.bottom + 8
        }
    }

    Item {
        id: leftColumn
        anchors {
            left: mainItem.left
            top: mainItem.top
            bottom: mainItem.bottom
        }
        width: mainItem.width / 4

        TextField {
            id: searchField
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            placeholderText: qsTr("Type to search...")
        }

        Component {
            id: itemDelegate

            ListItem {
                id: wrapper
                checked: ListView.isCurrentItem
                enabled: true

                Label {
                    text: label
                    font.weight: Font.Bold
                }
            }
        }

        ListView {
            id: categoriesList
            anchors {
                left: searchField.left
                top: searchField.bottom
                right: categoriesScrollBar.visible ? categoriesScrollBar.left : parent.right
                bottom: parent.bottom
                topMargin: 16
            }
            orientation: ListView.Vertical
            focus: true
            clip: true
            model: AppChooserCategoriesModel {}
            delegate: itemDelegate
            highlight: Highlight{}
            highlightRangeMode: ListView.StrictlyEnforceRange

            Connections {
                target: searchField
                onTextChanged: {
                    // TODO: Put searchField.text somewhere => categoriesList.model.setQuery(searchField.text);
                }
            }
        }

        ScrollBar {
            id: categoriesScrollBar
            anchors {
                top: searchField.bottom
                right: parent.right
                bottom: parent.bottom
            }
            flickableItem: categoriesList
        }
    }

    Item {
        id: rightColumn
        anchors {
            left: leftColumn.right
            top: mainItem.top
            right: mainItem.right
            bottom: mainItem.bottom
        }

        GridView {
            id: grid
            anchors {
                left: parent.left
                top: parent.top
                right: gridScrollBar.visible ? gridScrollBar.left : parent.right
                bottom: parent.bottom
            }
            cacheBuffer: 1000
            cellWidth: theme.enormousIconSize
            cellHeight: theme.enormousIconSize
            clip: true
            model: VisualDataModel {
                id: visualModel

                model: AvailableApplicationsModel {
                    id: appsModel
                }
                delegate: AppChooserDelegate {
                    visualIndex: VisualDataModel.itemsIndex
                    icon: "image://desktoptheme/" + (iconName ? iconName : "unknown")
                    label: name

                    onClicked: {
                        // Launch the application and close the AppChooser
                        // TODO: Hide this window
                        appsModel.launchApplicationAt(visualIndex);
                    }
                }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad }
            }
        }

        ScrollBar {
            id: gridScrollBar
            anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
            }
            flickableItem: grid
        }
    }
}
