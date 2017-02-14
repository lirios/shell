/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini
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

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0 as FluidControls
import Fluid.Material 1.0 as FluidMaterial

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

    FluidMaterial.Ripple {
        anchors.fill: parent

        onClicked: {
            if (model.running) {
                compositor.activateShellSurfaces(model.appId)
                appLaunched() // Not really, but we want the same thing to happen
            } else {
                applicationManager.launch(model.appId)
            }
        }
    }
}
