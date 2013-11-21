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
import FluidUi 0.2 as FluidUi
import Hawaii.Shell 1.0
import Hawaii.Shell.Settings 1.0

Item {
    id: statusArea
    width: {
        if (orientation == ListView.Horizontal)
            return listView.headerItem.width + (listView.count * iconSize) +
                    (margin * (listView.count + 3));
        return iconSize;
    }
    height: {
        if (orientation == ListView.Horizontal)
            return iconSize;
        return listView.headerItem.height + (listView.count * iconSize) +
                (margin * (listView.count + 3));
    }
    onWindowChanged: windowConnection.target = window

    property alias orientation: listView.orientation
    readonly property int margin: 8
    readonly property int iconSize: 16

    property int alignment

    // Launcher window
    property var window

    QtObject {
        id: __priv

        property var dateTime: Shell.service("DateTime")
        property list<Item> indicators: [
            VolumeIndicator {},
            DateTimeIndicator {}
        ]
    }

    // Status menu
    StatusMenu {
        id: statusMenu
        indicators: __priv.indicators

        Component.onCompleted: movePopupMenus()
    }

    // Popup menus follows the Launcher window when its geometry changes
    Connections {
        id: windowConnection
        onXChanged: movePopupMenus()
        onYChanged: movePopupMenus()
        onWidthChanged: movePopupMenus()
        onHeightChanged: movePopupMenus()
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: __priv.indicators
        spacing: margin
        interactive: false
        header: Item {
            width: orientation == ListView.Horizontal ? timeLabel.paintedWidth + 2 * margin : listView.width
            height: orientation == ListView.Horizontal ? listView.height : timeLabel.paintedHeight + 2 * margin

            Label {
                id: timeLabel
                anchors.centerIn: parent
                font.bold: true
                font.pixelSize: iconSize * 0.7
                color: mouseArea.hover ? "white" : "#cdcdcd"

                Behavior on color {
                    ColorAnimation { easing.type: Easing.Linear; duration: 250 }
                }

                Connections {
                    target: __priv.dateTime
                    onDateTimeChanged: {
                        var format = orientation == ListView.Horizontal ? "HH:mm" : "HH<br>mm";
                        timeLabel.text = Qt.formatTime(__priv.dateTime.dateTime, format);
                    }
                }
            }
        }
        delegate: FluidUi.Icon {
            iconName: modelData.iconName
            width: iconSize
            height: iconSize
            color: mouseArea.hover ? "white" : "#cdcdcd"
            visible: modelData.iconVisible

            Behavior on color {
                ColorAnimation { easing.type: Easing.Linear; duration: 250 }
            }
        }
    }

    MouseArea {
        property bool hover: false

        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: hover = true
        onExited: hover = false
        onClicked: statusMenu.visible = !statusMenu.visible
    }

    function movePopupMenus() {
        if (typeof(window) == "undefined")
            return;

        switch (alignment) {
        case LauncherSettings.LeftAlignment:
            statusMenu.x = window.x + window.width;
            statusMenu.y = window.y + window.height - statusMenu.height;
            break;
        case LauncherSettings.RightAlignment:
            statusMenu.x = window.x - statusMenu.width;
            statusMenu.y = window.y + window.height - statusMenu.height;
            break;
        case LauncherSettings.TopAlignment:
            statusMenu.x = window.x + window.width;
            statusMenu.y = window.y + window.height;
            break;
        case LauncherSettings.BottomAlignment:
            statusMenu.x = window.x + window.width - statusMenu.width;
            statusMenu.y = window.y - statusMenu.height;
            break;
        }
    }
}
