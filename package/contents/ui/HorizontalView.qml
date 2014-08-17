/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import org.kde.plasma.core 2.0 as PlasmaCore

RowLayout {
    spacing: units.smallSpacing

    SystemPalette {
        id: palette
    }

    CategoriesView {
        orientation: Qt.Vertical
        onSelected: grid.model = model

        Layout.fillHeight: true
    }

    ColumnLayout {
        ItemGridView {
            readonly property int numPages: Math.ceil(count / numItemsPerPage)
            property int currentPage: 0

            id: grid

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            id: pageIndicator
            height: units.gridUnit

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: units.smallSpacing

                Repeater {
                    id: repeater
                    model: grid.numPages
                    delegate: Rectangle {
                        width: pageIndicator.height
                        height: pageIndicator.height
                        color: grid.currentPage === index ? palette.highlight : "#40000000"
                        radius: width
                        antialiasing: true

                        Label {
                            anchors.centerIn: parent
                            text: index + 1
                            color: palette.highlightedText
                            font.pixelSize: pageIndicator.height * 0.7
                            visible: grid.currentPage === index
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                grid.currentIndex = index * grid.numItemsPerPage + 1;
                                grid.currentPage = index;
                            }
                        }
                    }
                }
            }

            Layout.fillWidth: true
        }

        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
