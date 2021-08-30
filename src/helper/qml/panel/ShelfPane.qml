// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import Fluid.Effects 1.0 as FluidEffects

Pane {
    id: control

    property bool highlighted: false
    readonly property alias pressed: tapHandler.pressed

    signal clicked()

    padding: 0

    TapHandler {
        id: tapHandler

        onTapped: {
            control.clicked();
        }
    }

    background: Rectangle {
        anchors.fill: parent
        anchors.margins: control.Material.elevation
        radius: 2
        color: control.Material.dialogColor

        layer.enabled: control.Material.elevation > 0
        layer.effect: FluidEffects.Elevation {
            elevation: control.Material.elevation
        }

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            height: 2
            color: control.Material.accentColor
            opacity: control.highlighted ? 1.0 : 0.0

            Behavior on opacity {
                NumberAnimation { duration: 150 }
            }

            Behavior on color {
                ColorAnimation { duration: 150 }
            }
        }

        Ripple {
            width: parent.width
            height: parent.height
            anchor: control
            pressed: control.pressed
            clipRadius: 2
            active: control.pressed || control.visualFocus || control.hovered
            color: control.highlighted ? control.Material.highlightedRippleColor : control.Material.rippleColor
        }
    }
}
