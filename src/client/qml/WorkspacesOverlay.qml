/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Shell.Desktop 0.1
import Hawaii.Shell.Styles 0.1

Item {
    id: workspacesOverlay
    width: Math.min(400, Screen.width)
    height: Math.min(300, Screen.height)
    opacity: 0.0

    property int margin: 20

    Behavior on opacity {
        NumberAnimation { duration: 250 }
    }

    Connections {
        target: Shell
        onWindowActivated: opacity = 0.0
        onWorkspaceSwitched: {
            opacity = 1.0;
            timer.start();
        }
    }

    Timer {
        id: timer
        repeat: false
        interval: 2000
        onTriggered: opacity = 0.0
    }

    StyledItem {
        id: styledItem
        anchors.fill: parent
        style: Qt.createComponent("OverlayStyle.qml", styledItem)

        ColumnLayout {
            id: mainLayout
            anchors {
                fill: parent
                margins: margin
            }

            // TODO: Number of workspaces and rows/cols from settings
            GridLayout {
                id: grid
                rows: 2
                columns: 2

                readonly property int itemWidth: (workspacesOverlay.width / rows) - margin
                readonly property int itemHeight: (workspacesOverlay.height / columns) - margin

                Repeater {
                    model: Shell.workspaces

                    Rectangle {
                        color: modelData.active
                               ? Qt.lighter(styledItem.__style.panelColor1, 1.5)
                               : Qt.lighter(styledItem.__style.panelColor1, 1.1)
                        radius: 6

                        Layout.preferredWidth: grid.itemWidth
                        Layout.preferredHeight: grid.itemHeight
                    }
                }
            }
        }
    }
}
