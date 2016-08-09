/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import Fluid.Controls 1.0
import Hawaii.Mpris 1.0

ColumnLayout {
    property var player
    readonly property int trackLength: player ? player.metadata["mpris:length"]||0 / 1000 : 0
    readonly property int trackPosition: player ? player.position : 0

    spacing: Units.largeSpacing

    Row {
        spacing: Units.smallSpacing

        Item {
            width: Units.iconSizes.huge
            height: width

            Image {
                id: albumArt
                anchors.fill: parent
                source: player ? player.metadata["mpris:artUrl"] : ""
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectFit
                visible: status == Image.Ready
            }

            Icon {
                id: icon
                anchors.fill: parent
                name: player ? player.iconName : ""
                visible: !albumArt.visible && name !== ""
            }

            BusyIndicator {
                anchors.fill: parent
                visible: !albumArt.visible && !icon.visible
            }

            Layout.alignment: Qt.AlignHCenter
        }

        Column {
            spacing: Units.smallSpacing

            SubheadingLabel {
                id: titleLabel
                text: player && player.metadata["xesam:title"] ? player.metadata["xesam:title"] : qsTr("Unknown Title")
                font.weight: Font.Bold
                elide: Text.ElideRight
            }

            SubheadingLabel {
                text: player && player.metadata["xesam:artist"] ? player.metadata["xesam:artist"] : qsTr("Unknown Artist")
                elide: Text.ElideRight
            }
        }
    }

    ProgressBar {
        from: 0
        to: trackLength
        value: trackPosition

        Layout.fillWidth: true
    }

    Row {
        IconButton {
            ToolTip.text: qsTr("Previous")
            ToolTip.visible: hovered

            iconName: "av/skip_previous"
            iconSize: Units.iconSizes.smallMedium
            onClicked: {
                if (player)
                    player.previous();
            }
        }

        IconButton {
            ToolTip.text: player.status === "Playing" ? qsTr("Pause") : qsTr("Play")
            ToolTip.visible: hovered

            iconName: player ? (player.status === "Playing" ? "av/pause" : "av/play_arrow") : ""
            iconSize: Units.iconSizes.smallMedium
            onClicked: {
                if (player)
                    player.playPause();
            }
        }

        IconButton {
            ToolTip.text: qsTr("Next")
            ToolTip.visible: hovered

            iconName: "av/skip_next"
            iconSize: Units.iconSizes.smallMedium
            onClicked: {
                if (player)
                    player.next();
            }
        }

        Layout.alignment: Qt.AlignHCenter
    }
}
