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

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import GreenIsland 1.0
import DesktopShell 0.1
import FluidCore 1.0 as FluidCore
import FluidUi 1.0 as FluidUi

//Dialog {
ShellWindow {
    id: appChooserWindow
    color: "transparent"
    width: 320
    height: 240

    Component {
        id: itemDelegate

        FluidUi.ListItem {
            id: wrapper
            checked: ListView.isCurrentItem
            enabled: true

            Label {
                text: label
                font.weight: Font.Bold
            }
        }
    }

    FluidCore.FrameSvgItem {
        anchors.fill: parent
        imagePath: "dialogs/background"

        ColumnLayout {
            anchors {
                fill: parent
                leftMargin: parent.margins.left
                topMargin: parent.margins.top
                rightMargin: parent.margins.right
                bottomMargin: parent.margins.bottom
            }

            TextField {
                id: searchField
                placeholderText: qsTr("Search...")

                Layout.fillWidth: true
            }

            RowLayout {
                ScrollView {
                    ListView {
                        id: categoriesList
                        orientation: ListView.Vertical
                        model: AppChooserCategoriesModel {}
                        delegate: itemDelegate
                        highlight: FluidUi.Highlight {}
                        highlightRangeMode: ListView.StrictlyEnforceRange

                        Connections {
                            target: searchField
                            onTextChanged: {
                                // TODO: Put searchField.text somewhere => categoriesList.model.setQuery(searchField.text);
                            }
                        }
                    }

                    Layout.fillHeight: true
                }

                ScrollView {
                    GridView {
                        id: grid
                        cacheBuffer: 1000
                        cellWidth: theme.enormousIconSize
                        cellHeight: theme.enormousIconSize
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
                                    appsModel.launchApplicationAt(visualIndex);
                                    root.appChooser.visible = false;
                                }
                            }
                        }

                        displaced: Transition {
                            NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad }
                        }
                    }

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
