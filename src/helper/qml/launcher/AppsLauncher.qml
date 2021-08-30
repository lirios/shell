// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects

Popup {
    id: appsLauncher

    signal appLaunched()
    signal dismissed()

    modal: true

    // Not really sure why this is necessary, popup blows up in width when switching modes
    width: implicitWidth

    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    padding: 0
    rightPadding: 2

    Material.theme: Material.Light
    Material.accent: Material.Blue

    Connections {
        target: appsModel

        function onApplicationLaunched(appId) {
            searchText.text = "";
            appsLauncher.appLaunched();
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
                        layer.effect: FluidEffects.Elevation {
                            elevation: 2
                        }
                    }
                }

                RowLayout {
                    id: topRow

                    anchors {
                        fill: parent
                        margins: FluidControls.Units.smallSpacing
                    }

                    ToolButton {
                        id: frequentAppsButton

                        icon.source: FluidControls.Utils.iconUrl("action/history")
                        checkable: true
                        checked: frequentApps.count > 0
                        autoExclusive: true
                        onClicked: {
                            categories.currentIndex = 0
                            searchText.forceActiveFocus()
                        }
                    }

                    ToolButton {
                        id: allAppsButton

                        icon.source: FluidControls.Utils.iconUrl("navigation/apps")
                        checkable: true
                        checked: frequentApps.count === 0
                        autoExclusive: true
                        onClicked: {
                            categories.currentIndex = 0
                            searchText.forceActiveFocus()
                        }
                    }

                    ToolButton {
                        id: categoriesButton

                        icon.source: FluidControls.Utils.iconUrl("action/list")
                        checkable: true
                        autoExclusive: true
                        onClicked: {
                            categories.currentIndex = 0;
                            searchText.forceActiveFocus();
                        }
                    }

                    Item {
                        width: FluidControls.Units.smallSpacing
                    }

                    TextField {
                        id: searchText

                        placeholderText: qsTr("Type an application name...")
                        focus: true
                        onTextChanged: {
                            grid.query = text

                            if (frequentAppsButton.checked)
                                allAppsButton.checked = true;
                        }

                        Layout.fillWidth: true
                    }

                    Item {
                        Layout.fillWidth: true
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
                        margins: FluidControls.Units.smallSpacing
                    }

                    FrequentAppsView {
                        id: frequentApps

                        visible: frequentAppsButton.checked
                    }

                    CategoriesView {
                        id: categories

                        Layout.preferredWidth: FluidControls.Units.gu(10)
                        Layout.fillHeight: true

                        clip: true
                        visible: categoriesButton.checked
                        onSelected: grid.filterByCategory(category)
                    }

                    ColumnLayout {
                        visible: !frequentAppsButton.checked

                        AppsGrid {
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
