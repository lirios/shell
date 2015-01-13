/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2012 Alex Merry <alex.merry@kdemail.net>
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

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import Hawaii.Themes 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import "../.."

Item {
    PlasmaCore.DataSource {
        id: mprisDataSource
        engine: "mpris2"
        connectedSources: current
        interval: 0

        property string current: "@multiplex"

        property bool hasMetadata: getHasMetadata()
        property string title: getMetadata("xesam:title", "")
        property string artist: getMetadata("xesam:artist", []).join(", ")
        property string album: getMetadata("xesam:album", "")
        property url pictureUrl: getMetadata("mpris:artUrl", "")
        property int trackLength: getMetadata("mpris:length", 0) / 1000
        property int trackPosition: getHasData() ? data[current]["Position"] : 0
        property string status: getHasData() ? data[current]["PlaybackStatus"] : "unknown"
        property bool canControl: getHasData() && data[current]["CanControl"]

        function startOperation(source, op) {
            var service = mprisDataSource.serviceForSource(source);
            var operation = service.operationDescription(op);
            return service.startOperationCall(operation);
        }

        function openMediaPlayer() {
            startOperation(current, "Raise");
        }

        function playPause() {
            startOperation(current, "PlayPause");
        }

        function prevTrack() {
            startOperation(current, "Previous");
        }

        function nextTrack() {
            startOperation(current, "Next");
        }

        function getHasData() {
            return data[current] !== undefined &&
                    data[current]["PlaybackStatus"] !== undefined;
        }

        function getHasMetadata() {
            return data[current] !== undefined &&
                    data[current]["Metadata"] !== undefined &&
                    data[current]["Metadata"]["mpris:trackid"] !== undefined;
        }

        function getMetadata(entry, def) {
            if (hasMetadata && data[current]["Metadata"][entry] !== undefined)
                return data[current]["Metadata"][entry];
            return def;
        }
    }

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
                    source: mprisDataSource.pictureUrl
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
                    text: mprisDataSource.title
                    color: Theme.palette.panel.textColor
                    font.bold: true
                    font.pointSize: 14
                }

                Label {
                    text: mprisDataSource.artist
                    color: Theme.palette.panel.textColor
                    font.pointSize: 10
                }
            }
        }

        ProgressBar {
            minimumValue: 0
            maximumValue: mprisDataSource.trackLength
            value: mprisDataSource.trackPosition

            Layout.fillWidth: true
        }

        Row {
            ToolButton {
                iconName: "media-skip-backward-symbolic"
                tooltip: qsTr("Previous")
                onClicked: mprisDataSource.prevTrack()
            }

            ToolButton {
                iconName: mprisDataSource.status == "Playing" ? "media-playback-pause" : "media-playback-start"
                tooltip: qsTr(mprisDataSource.status == "Playing" ? "Pause" : "Play")
                onClicked: mprisDataSource.playPause()
            }

            ToolButton {
                iconName: "media-skip-forward-symbolic"
                tooltip: qsTr("Next")
                onClicked: mprisDataSource.nextTrack()
            }

            Layout.alignment: Qt.AlignHCenter
        }
    }
}
