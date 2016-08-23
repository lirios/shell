/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

import QtQuick 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Material 1.0

Popup {
    id: launcherPopOver

    signal appLaunched()
    signal dismissed()

    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    implicitWidth: mainLayout.implicitWidth +
                   launcherPopOver.leftPadding + launcherPopOver.rightPadding +
                   launcherPopOver.leftMargin + launcherPopOver.rightMargin
    implicitHeight: mainLayout.implicitHeight +
                    launcherPopOver.topPadding + launcherPopOver.bottomPadding +
                    launcherPopOver.topMargin + launcherPopOver.bottomMargin

    padding: 0
    rightPadding: 2

    Material.theme: Material.Light
    Material.accent: Material.Blue

    Rectangle {
        radius: 2
        color: "#eee"
        width: mainLayout.width
        height: mainLayout.height

        ColumnLayout {
            id: mainLayout

            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: topRow.implicitHeight + 2 * topRow.anchors.margins

                color: "white"
                radius: 2

                Item {
                    width: parent.width
                    height: parent.height + 20
                    y: parent.radius
                    clip: true

                    Rectangle {
                        x: -5
                        y: -parent.y
                        width: parent.width - 2 * x
                        height: parent.parent.height

                        layer.enabled: true
                        layer.effect: ElevationEffect {
                            elevation: 2
                        }
                    }
                }

                RowLayout {
                    id: topRow
                    anchors {
                        fill: parent
                        margins: Units.smallSpacing
                    }

                    IconButton {
                        id: viewCategoriesButton
                        iconName: "action/view_list"
                        checkable: true
                        checked: false
                        autoExclusive: true
                        onClicked: categories.currentIndex = 0
                    }

                    IconButton {
                        id: viewPagedButton
                        iconName: "action/view_module"
                        checkable: true
                        checked: true
                        autoExclusive: true
                        onClicked: categories.currentIndex = 0
                    }

                    Item {
                        width: Units.smallSpacing
                    }

                    TextField {
                        id: searchText
                        placeholderText: qsTr("Type an application name...")
                        focus: true
                        onTextChanged: grid.query = text

                        Layout.fillWidth: true
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    LauncherShutdownActions {
                        id: shutdownActions

                        Layout.alignment: Qt.AlignHCenter
                    }

                    Layout.fillWidth: true
                }
            }

            Item {
                Layout.preferredWidth: contentRow.implicitWidth + 2 * contentRow.anchors.margins
                Layout.preferredHeight: contentRow.implicitHeight + 2 * contentRow.anchors.margins

                RowLayout {
                    id: contentRow

                    anchors {
                        fill: parent
                        margins: Units.smallSpacing
                    }

                    LauncherCategories {
                        id: categories
                        clip: true
                        width: Units.gu(10)
                        visible: viewCategoriesButton.checked
                        onSelected: grid.filterByCategory(category)

                        Layout.fillHeight: true
                    }

                    ColumnLayout {
                        LauncherGridView {
                            id: grid
                            onAppLaunched: {
                                launcherPopOver.appLaunched()
                                searchText.text = ""
                            }
                        }

                        PageIndicator {
                            id: pageIndicator
                            count: grid.pages
                            currentIndex: grid.currentPage
                            onCurrentIndexChanged: grid.currentPage = currentIndex

                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
            }
        }
    }
}
