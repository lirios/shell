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

    // Not really sure why this is necessary, popup blows up in width when switching modes
    width: implicitWidth

    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    padding: 0
    rightPadding: 2

    Material.theme: Material.Light
    Material.accent: Material.Blue

    Connections {
        target: applicationManager
        onApplicationLaunched: {
            searchText.text = ""
            launcherPopOver.appLaunched()
        }
    }

    Rectangle {
        radius: 2
        color: "#eee"
        implicitWidth: mainLayout.width
        implicitHeight: mainLayout.height

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
                        id: frequentAppsButton
                        iconName: "action/history"
                        checkable: true
                        checked: frequentApps.count > 0
                        autoExclusive: true
                        onClicked: {
                            categories.currentIndex = 0
                            searchText.forceActiveFocus()
                        }
                    }

                    IconButton {
                        id: allAppsButton
                        iconName: "navigation/apps"
                        checkable: true
                        checked: frequentApps.count === 0
                        autoExclusive: true
                        onClicked: {
                            categories.currentIndex = 0
                            searchText.forceActiveFocus()
                        }
                    }

                    IconButton {
                        id: categoriesButton
                        iconName: "action/list"
                        checkable: true
                        autoExclusive: true
                        onClicked: {
                            categories.currentIndex = 0
                            searchText.forceActiveFocus()
                        }
                    }

                    Item {
                        width: Units.smallSpacing
                    }

                    TextField {
                        id: searchText
                        placeholderText: qsTr("Type an application name...")
                        focus: true
                        onTextChanged: {
                            grid.query = text

                            if (frequentAppsButton.checked)
                                allAppsButton.checked = true
                        }

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

                    FrequentAppsView {
                        id: frequentApps

                        visible: frequentAppsButton.checked
                    }

                    LauncherCategories {
                        id: categories

                        Layout.preferredWidth: Units.gu(10)
                        Layout.fillHeight: true

                        clip: true
                        visible: categoriesButton.checked
                        onSelected: grid.filterByCategory(category)
                    }

                    ColumnLayout {
                        visible: !frequentAppsButton.checked

                        LauncherGridView {
                            id: grid
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
