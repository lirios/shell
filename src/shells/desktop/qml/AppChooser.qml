/****************************************************************************
 * This file is part of Hawaii Shell.
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
import Hawaii.Shell.Styles 0.1
import Hawaii.Shell.Desktop 0.1

Item {
    id: appChooser

    property var window

    ColumnLayout {
        anchors {
            fill: parent
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
                    model: XdgCategoriesModel {}
                    delegate: Item {
                        id: wrapper
                        //checked: ListView.isCurrentItem
                        //enabled: true
                        width: ListView.width
                        height: label.paintedHeight

                        Label {
                            id: label
                            anchors.fill: parent
                            text: model.label
                            font.weight: Font.Bold
                        }
                    }
                    highlight: Rectangle {
                        color: "lightsteelblue"
                    }
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
                    cellWidth: 108
                    cellHeight: 108
                    model: VisualDataModel {
                        id: visualModel

                        model: ApplicationsModel {
                            id: appsModel
                        }
                        delegate: AppChooserDelegate {
                            visualIndex: VisualDataModel.itemsIndex
                            //icon: "image://desktoptheme/" + (iconName ? iconName : "unknown")
                            icon: "image://appicon/" + iconName
                            label: name
                            onClicked: {
                                // Launch the application and close the AppChooser
                                var item = appsModel.get(visualIndex);
                                window.visible = false;
                                item.launch();
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
