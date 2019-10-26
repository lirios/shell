/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.10
import QtQuick.Window 2.9
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Liri.WaylandClient 1.0 as WaylandClient
import Fluid.Controls 1.0 as FluidControls

Window {
    id: osdWindow

    property string iconName
    property alias value: progress.value
    property alias text: label.text

    flags: Qt.Window | Qt.BypassWindowManagerHint

    width: 256
    height: 256

    color: "transparent"

    WaylandClient.WlrLayerSurfaceV1 {
        shell: layerShell
        layer: WaylandClient.WlrLayerShellV1.OverlayLayer
        showOnAllScreens: false
        window: osdWindow
        keyboardInteractivity: false
        nameSpace: "osd"

        onConfigured: {
            ackConfigure(serial);
            console.debug("Acked configure for osd with serial", serial);
            addMask(Qt.rect(0, 0, osdWindow.width, osdWindow.height));
            osdWindow.visible = true;
        }
        onClosed: {
            this.destroy();
            osdWindow.close();
        }
    }

    Rectangle {
        Material.theme: Material.Dark
        Material.accent: Material.LightBlue

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
                visible: label.text == ""

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
