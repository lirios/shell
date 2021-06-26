// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15 as MaterialImpl
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects
import "launcher"
import "../launcher" as Launcher

Rectangle {
    readonly property int iconSize: height >= 40 ? 56 * 0.36 : height * 0.45

    width: height
    height: parent.height

    color: Material.dialogColor
    radius: 2

    layer.enabled: !panel.maximized
    layer.effect: FluidEffects.Elevation {
        elevation: 8
    }

    AppsIcon {
        anchors.centerIn: parent
    }

    TapHandler {
        id: tapHandler

        onTapped: {
            popover.open();
        }
    }

    MaterialImpl.Ripple {
        anchors.fill: parent
        clip: visible
        pressed: tapHandler.pressed
        anchor: parent
        color: Material.rippleColor
    }

    Rectangle {
        id: highlight

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: 2
        color: Material.accentColor
        opacity: popover.visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: FluidControls.Units.mediumDuration
            }
        }

        Behavior on color {
            ColorAnimation {
                duration: FluidControls.Units.mediumDuration
            }
        }
    }

    Launcher.LauncherPopOver {
        id: popover

        x: (parent.width - width)/2
        y: (parent.height - height)/2

        parent: desktop

        modal: true

        onAppLaunched: {
            close();
        }
    }
}
