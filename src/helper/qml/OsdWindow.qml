// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Liri.WaylandClient.LayerShell 1.0 as LayerShell
import Liri.ShellHelper 1.0 as ShellHelper
import Fluid.Controls 1.0 as FluidControls

Window {
    id: osdWindow

    property string iconName
    property alias value: progress.value
    property alias text: label.text
    property alias progressVisible: progress.visible

    width: 256
    height: 256

    screen: primaryScreen
    color: "transparent"
    visible: true

    Component.onCompleted: {
        windowMask.addRect(Qt.rect(0, 0, width, height));
    }

    LayerShell.LayerSurface {
        layer: LayerShell.LayerSurface.OverlayLayer
        keyboardInteractivity: LayerShell.LayerSurface.NoKeyboardInteractivity
        role: "osd"
    }

    ShellHelper.WindowMask {
        id: windowMask
    }

    Rectangle {
        Material.theme: Material.Dark
        Material.accent: Material.Blue

        anchors.fill: parent

        color: Material.dialogColor
        radius: 6
        antialiasing: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: FluidControls.Units.largeSpacing
            spacing: FluidControls.Units.smallSpacing

            FluidControls.Icon {
                id: icon

                source: osdWindow.iconName ? FluidControls.Utils.iconUrl(osdWindow.iconName) : ""
                size: FluidControls.Units.iconSizes.enormous

                Layout.alignment: Qt.AlignHCenter
            }

            ProgressBar {
                id: progress

                from: 0
                to: 100
                visible: false

                Layout.fillWidth: true
            }

            Label {
                id: label

                visible: !progress.visible && text != ""

                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
