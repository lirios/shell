// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import Fluid.Controls as FluidControls
import QtAccountsService as Accounts
import Liri.Device as LiriDevice

Dialog {
    id: panelMenu

    signal logoutRequested()
    signal lockRequested()
    signal shutdownRequested()

    Material.theme: Material.Dark

    leftPadding: 0
    rightPadding: 0
    topPadding: 2
    bottomPadding: 2

    width: Math.max(400, layout.implicitWidth)
    height: Math.max(400, layout.implicitHeight)

    modal: true
    dim: false

    Accounts.UserAccount {
        id: currentUser
    }

    StackView {
        id: stackView

        anchors.fill: parent
        clip: true

        initialItem: Page {
            padding: 0
            header: Pane {
                RowLayout {
                    anchors.fill: parent

                    FluidControls.CircleImage {
                        source: "file://" + currentUser.iconFileName
                        width: 48
                        height: 48
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    ToolButton {
                        icon.source: Qt.resolvedUrl("qrc:/qt/qml/LiriShellHelper/images/logout.svg")
                        onClicked: {
                            panelMenu.logoutRequested();
                        }

                        ToolTip.text: qsTr("Log out")
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.visible: hovered
                    }

                    ToolButton {
                        icon.source: FluidControls.Utils.iconUrl("action/lock")
                        onClicked: {
                            panelMenu.lockRequested();
                        }

                        ToolTip.text: qsTr("Lock Session")
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.visible: hovered
                    }

                    ToolButton {
                        icon.source: FluidControls.Utils.iconUrl("action/power_settings_new")
                        enabled: LiriDevice.LocalDevice.canPowerOff
                        onClicked: {
                            panelMenu.shutdownRequested();
                        }

                        ToolTip.text: qsTr("Power off")
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.visible: hovered
                    }
                }
            }

            ColumnLayout {
                id: layout

                anchors.fill: parent

                Repeater {
                    model: menuExtensionsModel

                    Loader {
                        Layout.fillWidth: true

                        asynchronous: true
                        sourceComponent: model.component
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }

    function pushToMenu(item) {
        stackView.push(item);
    }

    function popFromMenu() {
        stackView.pop();
    }
}
