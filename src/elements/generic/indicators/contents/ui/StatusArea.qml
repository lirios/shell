/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Shell 1.0
import Hawaii.Shell.Core 1.0
import Hawaii.Shell.Settings 1.0

Element {
    id: statusArea
    implicitWidth: {
        if (listView.orientation == ListView.Horizontal)
            return listView.headerItem.width + (listView.count * __priv.iconSize) +
                    (__priv.margin * (listView.count + 3));
        return __priv.iconSize;
    }
    implicitHeight: {
        if (listView.orientation == ListView.Horizontal)
            return __priv.iconSize;
        return listView.headerItem.height + (listView.count * __priv.iconSize) +
                (__priv.margin * (listView.count + 3));
    }

    QtObject {
        id: __priv

        readonly property int margin: 8
        readonly property int iconSize: 16

        property var dateTime: Shell.service("DateTime")
        property list<Item> indicators: [
            VolumeIndicator {},
            DateTimeIndicator {}
        ]
    }

    StatusMenu {
        id: statusMenu
        indicators: __priv.indicators
    }

    ListView {
        id: listView
        anchors.fill: parent
        orientation: view.formFactor === Types.Horizontal ? ListView.Horizontal : ListView.Vertical
        model: __priv.indicators
        spacing: __priv.margin
        interactive: false
        header: Item {
            width: listView.orientation == ListView.Horizontal ? timeLabel.paintedWidth + 2 * __priv.margin : listView.width
            height: listView.orientation == ListView.Horizontal ? listView.height : timeLabel.paintedHeight + 2 * __priv.margin

            Label {
                id: timeLabel
                anchors.centerIn: parent
                font.bold: true
                font.pixelSize: __priv.iconSize * 0.7
                color: mouseArea.hover ? "white" : "#cdcdcd"

                Behavior on color {
                    ColorAnimation { easing.type: Easing.Linear; duration: 250 }
                }

                Connections {
                    target: __priv.dateTime
                    onDateTimeChanged: {
                        var format = listView.orientation == ListView.Horizontal ? "HH:mm" : "HH<br>mm";
                        timeLabel.text = Qt.formatTime(__priv.dateTime.dateTime, format);
                    }
                }
            }
        }
        delegate: FluidUi.Icon {
            iconName: modelData.iconName
            width: __priv.iconSize
            height: __priv.iconSize
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
}
