/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import "../components" as CustomComponents

Popup {
    signal appLaunched()
    signal dismissed()

    id: launcherPopOver
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    implicitWidth: mainLayout.implicitWidth +
                   launcherPopOver.leftPadding + launcherPopOver.rightPadding +
                   launcherPopOver.leftMargin + launcherPopOver.rightMargin
    implicitHeight: mainLayout.implicitHeight +
                    launcherPopOver.topPadding + launcherPopOver.bottomPadding +
                    launcherPopOver.topMargin + launcherPopOver.bottomMargin

    Material.theme: Material.Dark

    ColumnLayout {
        id: mainLayout
        spacing: Units.smallSpacing

        RowLayout {
            CustomComponents.ToolButton {
                id: viewCategoriesButton
                iconName: "view-more-symbolic"
                checkable: true
                checked: false
                autoExclusive: true
                onClicked: categories.currentIndex = 0
            }

            CustomComponents.ToolButton {
                id: viewPagedButton
                iconName: "view-paged-symbolic"
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

        RowLayout {
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
                    onAppLaunched: launcherPopOver.appLaunched()

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        onWheel: {
                            if (wheel.angleDelta.x > 0 || wheel.angleDelta.y > 0) {
                                // Go to the next page
                                if (grid.currentPage < grid.numPages - 1) {
                                    grid.currentPage++;
                                    grid.currentIndex = grid.currentPage * grid.numItemsPerPage + 1;
                                }
                            } else if (wheel.angleDelta.x < 0 || wheel.angleDelta.y < 0) {
                                // Go to the next page
                                if (grid.currentPage > 0) {
                                    grid.currentPage--;
                                    grid.currentIndex = grid.currentPage * grid.numItemsPerPage + 1;
                                }
                            }

                            wheel.accepted = true;
                        }
                    }

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                PageIndicator {
                    id: pageIndicator
                    count: grid.numPages
                    currentIndex: grid.currentPage
                    onCurrentIndexChanged: grid.currentPage = currentIndex

                    Layout.alignment: Qt.AlignHCenter
                }

                Item {
                    height: Units.smallSpacing
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
