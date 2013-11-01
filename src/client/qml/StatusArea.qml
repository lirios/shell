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

Item {
    id: statusArea

    property alias orientation: listView.orientation
    readonly property int margin: 8
    readonly property int iconSize: 16

    signal clicked()

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

    QtObject {
        id: __priv

        property var dateTime: Shell.service("DateTime")
    }

    ListModel {
        id: indicatorsModel

        ListElement { icon: "audio-volume-muted-symbolic" }
        ListElement { icon: "battery-good-charging-symbolic" }
        ListElement { icon: "network-wireless-signal-good-symbolic" }
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: indicatorsModel
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
                color: "white"

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
            iconName: model.icon
            width: iconSize
            height: iconSize
            color: "white"

            Behavior on color {
                ColorAnimation { easing.type: Easing.Linear; duration: 250 }
            }
        }
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 250 }
            NumberAnimation { property: "scale"; from: 0.0; to: 1.0; duration: 150 }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 250; easing.type: Easing.OutBounce }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: statusArea.clicked()
    }
}
