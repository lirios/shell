/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Shell.Applications 1.0

Item {
    property Item containment

    id: desktopView
    onContainmentChanged: {
        containment.anchors.left = desktopView.left;
        containment.anchors.top = desktopView.top;
        containment.anchors.right = desktopView.right;
        containment.anchors.bottom = desktopView.bottom;
        containment.visible = false;
    }

    readonly property int itemSize: 192
    readonly property int numRows: 5
    readonly property int numColumns: 3

    ColumnLayout {
        TextField {
            placeholderText: qsTr("Search")

            Layout.minimumWidth: 250
            Layout.alignment: Qt.AlignHCenter
        }

        GridView {
            id: grid
            cacheBuffer: 100
            cellWidth: itemSize
            cellHeight: itemSize
            width: itemSize * numRows
            height: itemSize * numColumns
            clip: true
            snapMode: GridView.SnapOneRow
            flow: GridView.TopToBottom
            preferredHighlightBegin: 0
            preferredHighlightEnd: 0
            highlightRangeMode: GridView.StrictlyEnforceRange
            highlightFollowsCurrentItem: true
            model: VisualDataModel {
                id: visualModel
                model: ApplicationsModel {
                    id: appsModel
                }
                delegate: IconDelegate {
                    icon: "image://appicon/" + iconName
                    label: name
                    onClicked: {
                        // Launch the application
                        appsModel.launch(VisualDataModel.itemsIndex);
                    }
                }
            }
        }
    }

    Component.onCompleted: appsModel.populate()
}
