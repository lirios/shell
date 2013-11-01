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
import QtQuick.Layouts 1.0
import Hawaii.Shell 1.0
import Hawaii.Shell.Settings 1.0
import Hawaii.Shell.Styles 1.0

Element {
    id: launcherContainer

    // Tile size
    property alias tileSize: launcherView.tileSize

    // Alignment and orientation
    property alias alignment: launcherView.alignment
    property alias orientation: launcherView.orientation

    // Size
    property int size: tileSize +
                       styledItem.__style.padding.left +
                       styledItem.__style.padding.top +
                       styledItem.__style.padding.right +
                       styledItem.__style.padding.bottom

    // Number of items
    property alias count: launcherView.count

    // Propagate window pointer to the view
    property alias window: launcherView.window

    StyledItem {
        id: styledItem
        anchors.fill: parent
        style: Qt.createComponent(StyleSettings.path + "/LauncherStyle.qml", launcherContainer)

        GridLayout {
            anchors {
                fill: parent
                leftMargin: styledItem.__style.padding.left
                topMargin: styledItem.__style.padding.top
                rightMargin: styledItem.__style.padding.right
                bottomMargin: styledItem.__style.padding.bottom
            }
            rows: orientation == ListView.Horizontal ? 1 : 2
            columns: orientation == ListView.Horizontal ? 2 : 1

            LauncherView {
                id: launcherView
                orientation: {
                    switch (alignment) {
                    case LauncherSettings.BottomAlignment:
                        return ListView.Horizontal;
                    default:
                        return ListView.Vertical;
                    }
                }

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            StatusArea {
                id: statusArea
                orientation: launcherView.orientation
                onClicked: launcherView.showStatusMenu()

                Layout.alignment: orientation == ListView.Horizontal ? Qt.AlignRight | Qt.AlignVCenter : Qt.AlignBottom | Qt.AlignHCenter
            }
        }
    }
}
