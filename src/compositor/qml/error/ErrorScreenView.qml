// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls

Rectangle {
    Material.theme: Material.Dark

    color: Material.color(Material.Red)

    ColumnLayout {
        anchors.centerIn: parent

        spacing: 0

        FluidControls.Icon {
            source: FluidControls.Utils.iconUrl("alert/warning")
            size: 96

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.minimumHeight: 8
        }

        FluidControls.DisplayLabel {
            text: qsTr("Oh no!")
            color: Material.primaryTextColor

            Layout.alignment: Qt.AlignHCenter
        }

        FluidControls.SubheadingLabel {
            text: qsTr("Something went wrong and the desktop failed to load.")
            color: Material.primaryTextColor

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.minimumHeight: 24
        }

        Button {
            text: qsTr("Quit")
            onClicked: liriCompositor.quit()

            Layout.alignment: Qt.AlignHCenter

            Material.background: "white"
            Material.theme: Material.Light
            Material.elevation: 2
        }
    }
}
