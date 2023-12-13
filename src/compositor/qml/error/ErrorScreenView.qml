// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import Fluid.Controls as FluidControls

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
