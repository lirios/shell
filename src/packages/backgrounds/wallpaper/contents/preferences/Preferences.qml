/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.1
import QtQuick.Window 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import Fluid.Core 1.0 as FluidCore
import Hawaii.SystemPreferences.Background 1.0
import Hawaii.Configuration 1.0

Item {
    property int columns: 6
    property int cellPadding: 5
    property real aspectRatio: Screen.width / Screen.height

    id: root
    width: 640
    height: 480

    SystemPalette {
        id: palette
    }

    Configuration {
        id: settings
        category: "shell/backgrounds/org.hawaii.backgrounds.wallpaper"

        property color color: "#336699"
        property url wallpaperUrl: FluidCore.StandardPaths.locateFile(FluidCore.StandardPaths.GenericDataLocation, "backgrounds/hawaii/Also_Calm.png")
        property int fillMode: Image.Stretch
    }

    Configuration {
        id: shellSettings
        category: "shell"

        property string background
    }

    ColumnLayout {
        anchors.fill: parent

        ScrollView {
            GridView {
                id: gridView
                model: BackgroundsModel {
                    id: backgroundsModel
                }
                cellWidth: parent.width / columns
                cellHeight: cellWidth / aspectRatio
                currentIndex: -1
                highlightMoveDuration: 0
                delegate: Item {
                    width: gridView.cellWidth
                    height: gridView.cellHeight

                    Image {
                        anchors {
                            fill: parent
                            margins: cellPadding
                        }
                        source: model.fileName ? "file://" + model.fileName : ""
                        sourceSize.width: width
                        sourceSize.height: height
                        width: parent.width - cellPadding * 2
                        height: parent.height - cellPadding * 2
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true

                        BusyIndicator {
                            anchors.centerIn: parent
                            running: parent.status == Image.Loading
                        }

                        MouseArea {
                            id: mouse
                            anchors.fill: parent
                            onClicked: {
                                gridView.currentIndex = index;
                                settings.wallpaperUrl = "file://" + backgroundsModel.get(index);
                                shellSettings.background = "org.hawaii.backgrounds.wallpaper";
                            }
                        }
                    }
                }
                highlight: Rectangle {
                    radius: 4
                    color: palette.highlight
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        GridLayout {
            columns: 2

            Label {
                text: qsTr("Fill Mode:")
                horizontalAlignment: Qt.AlignRight
            }

            ComboBox {
                model: [
                    qsTr("Stretched"),
                    qsTr("Scaled"),
                    qsTr("Cropped"),
                    qsTr("Centered"),
                    qsTr("Tiled")
                ]
                currentIndex: mapFillModeToIndex(settings.fillMode)
                onActivated: settings.fillMode(mapIndexToFillMode(index))
            }

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        }
    }

    function mapFillModeToIndex(fillMode) {
        switch (fillMode) {
        case Image.PreserveAspectFit:
            return 1;
        case Image.PreserveAspectCrop:
            return 2;
        case Image.Pad:
            return 3;
        case Image.Tile:
            return 4;
        default:
            break;
        }

        return 0;
    }

    function mapIndexToFillMode(index) {
        switch (index) {
        case 1:
            return Image.PreserveAspectFit;
        case 2:
            return Image.PreserveAspectCrop;
        case 3:
            return Image.Pad;
        case 4:
            return Image.Tile;
        default:
            break;
        }

        return Image.Stretch;
    }

    Component.onCompleted: {
        // Load backgrounds
        backgroundsModel.addStandardPaths();
        //backgroundsModel.addUserPaths();

        // Load settings
        for (var i = 0; i < gridView.count; i++) {
            var url = "file://" + gridView.model.get(i);
            if (url === settings.wallpaperUrl.toString()) {
                gridView.currentIndex = i;
                return;
            }
        }
        gridView.currentIndex = -1;
    }
}
