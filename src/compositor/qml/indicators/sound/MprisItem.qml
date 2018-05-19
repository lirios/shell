/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
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

import QtQuick 2.10
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import Fluid.Controls 1.0 as FluidControls
import Liri.Mpris 1.0

FluidControls.ListItem {
    property var player

    topPadding: 16
    bottomPadding: 8

    contentItem: ColumnLayout {
        RowLayout {
            Layout.fillWidth: true

            spacing: FluidControls.Units.smallSpacing * 2

            Item {
                Layout.preferredHeight: 40
                Layout.preferredWidth: 40

                Image {
                    id: albumArt

                    anchors.fill: parent

                    source: player ? player.metadata["mpris:artUrl"] : ""
                    // TODO: Multiply by screen ratio
                    sourceSize.width: width
                    sourceSize.height: height
                    fillMode: Image.PreserveAspectFit
                    visible: status == Image.Ready
                }

                FluidControls.Icon {
                    id: icon
                    anchors.centerIn: parent
                    name: player ? player.iconName : ""
                    source: player ? "" : FluidControls.Utils.iconUrl("image/music_note")
                    visible: !albumArt.visible
                    size: 40
                }
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.fillWidth: true

                spacing: 0

                FluidControls.SubheadingLabel {
                    Layout.fillWidth: true

                    color: Material.primaryTextColor
                    text: player && player.metadata["xesam:title"] || qsTr("Unknown Title")
                    elide: Text.ElideRight
                }

                FluidControls.BodyLabel {
                    Layout.fillWidth: true

                    color: Material.secondaryTextColor
                    elide: Text.ElideRight
                    wrapMode: Text.WordWrap

                    text: player && player.metadata["xesam:artist"] || qsTr("Unknown Artist")
                }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter

            ToolButton {
                ToolTip.text: qsTr("Previous")
                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                ToolTip.visible: hovered

                icon.source: FluidControls.Utils.iconUrl("av/skip_previous")
                onClicked: {
                    if (player)
                        player.previous();
                }
            }

            ToolButton {
                ToolTip.text: player.status === "Playing" ? qsTr("Pause") : qsTr("Play")
                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                ToolTip.visible: hovered

                icon.source: FluidControls.Utils.iconUrl(player ? (player.status === "Playing" ? "av/pause" : "av/play_arrow") : "")
                onClicked: {
                    if (player)
                        player.playPause();
                }
            }

            ToolButton {
                ToolTip.text: qsTr("Next")
                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                ToolTip.visible: hovered

                icon.source: FluidControls.Utils.iconUrl("av/skip_next")
                onClicked: {
                    if (player)
                        player.next();
                }
            }
        }
    }
}
