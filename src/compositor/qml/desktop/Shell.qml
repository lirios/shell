// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml.Models 2.15
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Core 1.0 as FluidCore
import Fluid.Controls 1.0 as FluidControls
import QtAccountsService 1.0 as Accounts
import Liri.Device 1.0 as LiriDevice
import Liri.Session 1.0 as Session
import Liri.private.shell 1.0 as P
import "../panel"

Item {
    id: shell

    readonly property alias panel: panel

    Accounts.UserAccount {
        id: currentUser
    }

    Instantiator {
        model: P.ExtensionsModel {}
        delegate: Loader {
            property string name: model.name

            source: model.url
            onStatusChanged: {
                if (status === Loader.Error)
                    console.error("Failed to load status area extension", name);
            }
        }
        onObjectAdded: {
            if (object.name && object.item) {
                console.info("Loading status area extension:", object.name);

                if (object.item.indicator)
                    indicatorsModel.append({"component": object.item.indicator});
                if (object.item.menu)
                    menuExtensionsModel.append({"component": object.item.menu});
            }
        }
    }

    ListModel {
        id: indicatorsModel
    }

    ListModel {
        id: menuExtensionsModel
    }

    Panel {
        id: panel
    }

    Dialog {
        id: panelMenu

        Material.theme: Material.Dark

        parent: shell

        x: output.availableGeometry.x + output.availableGeometry.width - width - FluidControls.Units.smallSpacing
        y: output.availableGeometry.y + output.availableGeometry.height - height - FluidControls.Units.smallSpacing

        leftPadding: 0
        rightPadding: 0
        topPadding: 2
        bottomPadding: 2

        width: Math.max(400, layout.implicitWidth)
        height: Math.max(400, layout.implicitHeight)

        modal: true
        dim: false

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

                        ToolButton {
                            icon.source: Qt.resolvedUrl("../images/logout.svg")
                            onClicked: {
                                panelMenu.close();
                                shellHelper.requestLogout();
                            }

                            ToolTip.text: qsTr("Log out")
                            ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                            ToolTip.visible: hovered
                        }

                        ToolButton {
                            icon.source: FluidControls.Utils.iconUrl("action/lock")
                            onClicked: {
                                panelMenu.close();
                                Session.SessionManager.lock();
                            }

                            ToolTip.text: qsTr("Lock Session")
                            ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                            ToolTip.visible: hovered
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        ToolButton {
                            icon.source: FluidControls.Utils.iconUrl("action/power_settings_new")
                            enabled: LiriDevice.LocalDevice.canPowerOff
                            onClicked: {
                                panelMenu.close();
                                shellHelper.requestShutdown();
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
    }

    function pushToMenu(item) {
        stackView.push(item);
    }

    function popFromMenu() {
        stackView.pop();
    }
}
