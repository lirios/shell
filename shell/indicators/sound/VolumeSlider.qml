/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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
import Fluid.Controls 1.0

ListItem {
    visible: volumeControl.visible

    leftItem: IconButton {
        anchors.centerIn: parent

        iconName: "av/volume_off"
        onClicked: volumeControl.setMinimumVolume()
    }

    rightItem: IconButton {
        anchors.centerIn: parent

        iconName: "av/volume_up"
        onClicked: volumeControl.setMaximumVolume()
    }

    // Remember that position is in the 0:1 range and value is in the 0:100 range
    secondaryItem: Slider {
        id: slider
        from: volumeControl.from
        to: volumeControl.to
        stepSize: volumeControl.stepSize
        onPositionChanged: {
            // Change volume on if the handle is pressed to avoid loops
            if (slider.pressed)
                volumeControl.setVolumePercentage(slider.position * 100.0)
        }

        // Set slider value as soon as the default sink is available
        Connections {
            target: volumeControl
            onVolumeChanged: {
                // Change the slider value for external events to avoid loops
                if (!slider.pressed)
                    slider.value = volume
            }

            Component.onCompleted: slider.value = volumeControl.getVolumePercentage()
        }

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
    }
}
