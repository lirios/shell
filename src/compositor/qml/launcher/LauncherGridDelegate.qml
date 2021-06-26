// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls

Rectangle {
    property bool hovered: false

    color: hovered ? Qt.rgba(0,0,0,0.1) : "transparent"
    radius: 2
    antialiasing: true

    Behavior on color {
        ColorAnimation { duration: FluidControls.Units.mediumDuration }
    }

    Rectangle {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }

        width: 50
        height: 2
        color: Material.accentColor
        visible: model.running
    }

    FluidControls.Icon {
        id: icon
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            bottom: label.top
            margins: 2 * FluidControls.Units.smallSpacing
        }
        size: height
        name: model.iconName
    }

    Label {
        id: label

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: FluidControls.Units.smallSpacing
        }
        text: model.name
        elide: Text.ElideRight
        textFormat: Text.PlainText
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - 2 * FluidControls.Units.smallSpacing
    }

    FluidControls.Ripple {
        anchors.fill: parent

        onClicked: {
            if (model.running) {
                liriCompositor.activateShellSurfaces(model.appId)
                appLaunched() // Not really, but we want the same thing to happen
            } else {
                applicationManager.launch(model.appId)
            }
        }
    }
}
