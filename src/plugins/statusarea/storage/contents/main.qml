// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Fluid.Controls as FluidControls
import Liri.Shell as Shell
import Liri.Storage as Storage

Shell.StatusAreaExtension {
    Storage.StorageModel {
        id: storageModel
    }

    Component {
        id: pageComponent

        Page {
            padding: 0
            header: RowLayout {
                ToolButton {
                    icon.source: FluidControls.Utils.iconUrl("navigation/arrow_back")
                    onClicked: {
                        popFromMenu();
                    }
                }

                FluidControls.TitleLabel {
                    text: qsTr("Storage")

                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                }
            }

            ScrollView {
                anchors.fill: parent
                clip: true

                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                ListView {
                    model: Storage.StorageModel {}
                    delegate: FluidControls.ListItem {
                        icon.name: model.iconName + "-symbolic"
                        text: model.name
                        onClicked: SessionInterface.launchCommand("xdg-open file://" + model.filePath)

                        rightItem: ToolButton {
                            anchors.centerIn: parent

                            ToolTip.text: model.mounted ? qsTr("Eject") : qsTr("Mount")
                            ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                            ToolTip.visible: hovered

                            icon.source: Qt.resolvedUrl("images/" + (model.mounted ? "eject.svg" : "disk.svg"))
                            onClicked: model.mounted ? model.storageDevice.unmount() : model.storageDevice.mount()
                        }
                    }
                }
            }
        }
    }

    indicator: Shell.Indicator {
        title: qsTr("Storage")
        iconSource: Qt.resolvedUrl("images/harddisk.svg")
        visible: storageModel.count > 0
    }

    menu: FluidControls.ListItem {
        icon.source: FluidControls.Utils.iconUrl("device/storage")
        text: qsTr("Storage")
        onClicked: {
            pushToMenu(pageComponent);
        }
    }
}
