/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
  *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import Hawaii.Controls 1.0 as Controls
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaii.mpris2 0.1

ColumnLayout {
    property var player
    readonly property int trackLength: player ? player.metadata["mpris:length"]||0 / 1000 : 0
    readonly property int trackPosition: player ? player.position : 0

    spacing: Themes.Units.largeSpacing

    Row {
        spacing: Themes.Units.smallSpacing

        Item {
            width: Themes.Units.iconSizes.huge
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

            Components.Icon {
                id: icon
                anchors.fill: parent
                iconName: player ? player.iconName : ""
                visible: !albumArt.visible && iconName != ""
            }

            BusyIndicator {
                anchors.fill: parent
                visible: !albumArt.visible && !icon.visible
            }

            Layout.alignment: Qt.AlignHCenter
        }

        Column {
            spacing: Themes.Units.smallSpacing

            Controls.Heading {
                level: 3
                text: player ? player.metadata["xesam:title"] : qsTr("Unknown Title")
                color: Themes.Theme.palette.panel.textColor
                font.weight: Font.Bold
                elide: Text.ElideRight
            }

            Controls.Heading {
                level: 4
                text: player ? player.metadata["xesam:artist"] : qsTr("Unknown Artist")
                color: Themes.Theme.palette.panel.textColor
                elide: Text.ElideRight
            }
        }
    }

    ProgressBar {
        minimumValue: 0
        maximumValue: trackLength
        value: trackPosition

        Layout.fillWidth: true
    }

    Row {
        ToolButton {
            iconName: "media-skip-backward-symbolic"
            tooltip: qsTr("Previous")
            onClicked: {
                if (player)
                    player.previous();
            }
        }

        ToolButton {
            iconName: player ? (player.status === "Playing" ? "media-playback-pause" : "media-playback-start") : ""
            tooltip: qsTr(player.status == "Playing" ? "Pause" : "Play")
            onClicked: {
                if (player)
                    player.playPause();
            }
        }

        ToolButton {
            iconName: "media-skip-forward-symbolic"
            tooltip: qsTr("Next")
            onClicked: {
                if (player)
                    player.next();
            }
        }

        Layout.alignment: Qt.AlignHCenter
    }
}
