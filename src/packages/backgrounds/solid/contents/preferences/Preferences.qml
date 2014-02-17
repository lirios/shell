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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import Hawaii.Configuration 1.0
import Hawaii.SystemPreferences.Background 1.0

Item {
    property int columns: 6
    property int cellPadding: 5
    property real aspectRatio: root.width / root.height

    id: root
    width: 800
    height: 600

    SystemPalette {
        id: palette
    }

    Configuration {
        id: settings
        category: "shell/backgrounds/org.hawaii.backgrounds.solid"

        property color color: "#336699"
    }

    Configuration {
        id: shellSettings
        category: "shell"

        property string background
    }

    ScrollView {
        anchors.fill: parent

        GridView {
            id: gridView
            model: ColorsModel {}
            cellWidth: parent.width / columns
            cellHeight: cellWidth / aspectRatio
            currentIndex: -1
            highlightMoveDuration: 0
            delegate: Item {
                width: gridView.cellWidth
                height: gridView.cellHeight

                Rectangle {
                    anchors {
                        fill: parent
                        margins: cellPadding
                    }
                    color: model.color

                    MouseArea {
                        id: mouse
                        anchors.fill: parent
                        onClicked: {
                            gridView.currentIndex = index;
                            settings.color = parent.color;
                            shellSettings.background = "org.hawaii.backgrounds.solid";
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

    Component.onCompleted: {
        // Load settings
        for (var i = 0; i < gridView.count; i++) {
            var color = gridView.model.get(i).color;
            if (settings.color === color) {
                gridView.currentIndex = i;
                return;
            }
        }
        gridView.currentIndex = -1;
    }
}
