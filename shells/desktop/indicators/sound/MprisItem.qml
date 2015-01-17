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
import Hawaii.Themes 1.0

Item {
    property var dataSource

    ColumnLayout {
        anchors.fill: parent
        spacing: units.smallSpacing

        Row {
            spacing: units.smallSpacing

            Item {
                width: units.iconSizes.huge
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
                spacing: units.smallSpacing

                Label {
                    text: dataSource ? dataSource.title : ""
                    color: Theme.palette.panel.textColor
                    font.bold: true
                    font.pointSize: 14
                }

                Label {
                    text: dataSource ? dataSource.artist : ""
                    color: Theme.palette.panel.textColor
                    font.pointSize: 10
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
}
