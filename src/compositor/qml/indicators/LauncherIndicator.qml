/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
