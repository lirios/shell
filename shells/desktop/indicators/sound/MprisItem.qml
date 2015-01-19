/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes

ColumnLayout {
    property var dataSource

    spacing: Themes.Units.largeSpacing

    Row {
        spacing: Themes.Units.smallSpacing

        Item {
            width: Themes.Units.iconSizes.huge
            height: width

            Image {
                id: albumArt
                anchors.fill: parent
                source: dataSource ? dataSource.pictureUrl : ""
                fillMode: Image.PreserveAspectFit
                visible: status == Image.Ready
            }

            BusyIndicator {
                anchors.fill: parent
                visible: !albumArt.visible
            }

            Layout.alignment: Qt.AlignHCenter
        }

        Column {
            spacing: Themes.Units.smallSpacing

            Controls.Heading {
                level: 3
                text: dataSource ? dataSource.title : ""
                color: Themes.Theme.palette.panel.textColor
                font.weight: Font.Bold
                elide: Text.ElideRight
            }

            Controls.Heading {
                level: 4
                text: dataSource ? dataSource.artist : ""
                color: Themes.Theme.palette.panel.textColor
                elide: Text.ElideRight
            }
        }
    }

    ProgressBar {
        minimumValue: 0
        maximumValue: dataSource ? dataSource.trackLength : 0
        value: dataSource ? dataSource.trackPosition : 0

        Layout.fillWidth: true
    }

    Row {
        ToolButton {
            iconName: "media-skip-backward-symbolic"
            tooltip: qsTr("Previous")
            onClicked: {
                if (dataSource)
                    dataSource.prevTrack();
            }
        }

        ToolButton {
            iconName: dataSource ? (dataSource.status === "Playing" ? "media-playback-pause" : "media-playback-start") : ""
            tooltip: qsTr(mprisDataSource.status == "Playing" ? "Pause" : "Play")
            onClicked: {
                if (dataSource)
                    dataSource.playPause();
            }
        }

        ToolButton {
            iconName: "media-skip-forward-symbolic"
            tooltip: qsTr("Next")
            onClicked: {
                if (dataSource)
                    dataSource.nextTrack();
            }
        }

        Layout.alignment: Qt.AlignHCenter
    }
}
