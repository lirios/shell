// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls

Rectangle {
    id: root

    property alias hovered: hoverHandler.hovered
    signal clicked()

    Material.theme: Material.Dark

    width: FluidControls.Units.iconSizes.smallMedium
    height: width
    radius: width * 0.5
    border.color: Qt.rgba(1, 1, 1, 0.35)
    border.width: FluidControls.Units.gu(0.05)
    color: Material.dialogColor
    antialiasing: true

    FluidControls.Icon {
        anchors.centerIn: parent
        source: FluidControls.Utils.iconUrl("navigation/close")
        color: "white"
        width: parent.width - FluidControls.Units.smallSpacing
        height: width
    }

    HoverHandler {
        id: hoverHandler
    }

    TapHandler {
        onTapped: {
            root.clicked();
        }
    }
}
