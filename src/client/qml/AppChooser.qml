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
    implicitWidth: mainLayout.implicitWidth
    implicitHeight: mainLayout.implicitHeight

    property var window
    readonly property int itemSize: 128
    readonly property int numRows: 4
    readonly property int numColumns: 4
    readonly property int numItemsPerPage: numRows * numColumns
    readonly property int numPages: Math.ceil(grid.count / numItemsPerPage)
    property int currentPage: 0
    property var palette: SystemPalette {}

    Connections {
        target: window
        onVisibleChanged: {
            currentPage = 0;
            grid.currentIndex = 0;
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        GridView {
            id: grid
            cacheBuffer: numItemsPerPage
            cellWidth: itemSize
            cellHeight: itemSize
            width: itemSize * numRows
            height: itemSize * numColumns
            clip: true
            snapMode: GridView.SnapOneRow
            flow: GridView.TopToBottom
            interactive: false
            preferredHighlightBegin: 0
            preferredHighlightEnd: 0
            highlightRangeMode: GridView.StrictlyEnforceRange
            highlightFollowsCurrentItem: true
            model: VisualDataModel {
                id: visualModel
                model: ApplicationsModel {
                    id: appsModel
                }
                delegate: AppChooserDelegate {
                    icon: "image://appicon/" + iconName
                    label: name
                    onClicked: {
                        // Launch the application and close the AppChooser
                        var item = appsModel.get(VisualDataModel.itemsIndex);
                        window.visible = false;
                        item.launch();
                    }
                }
            }
        }

        Item {
            id: pageIndicator
            width: grid.width
            height: 16

            Row {
                anchors.fill: parent

                Repeater {
                    id: repeater
                    model: numPages
                    delegate: Rectangle {
                        width: pageIndicator.width / numPages
                        height: pageIndicator.height
                        color: currentPage === index ? palette.highlight : "#40000000"
                        radius: 6
                        antialiasing: true

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                grid.currentIndex = index * numItemsPerPage + 1;
                                currentPage = index;
                            }
                        }
                    }
                }
            }
        }
    }
}
