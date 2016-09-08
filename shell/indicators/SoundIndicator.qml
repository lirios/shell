/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

import QtQuick 2.5
import QtQuick.Layouts 1.0
import Fluid.Controls 1.0
import Hawaii.Shell 1.0
import Hawaii.Mpris 1.0
import Hawaii.Settings 1.0 as Settings
import "sound" as SoundIndicator

Indicator {
    id: indicator

    title: qsTr("Sound")
    iconName: volumeControl.getIconName()
    tooltip: volumeControl.visible
            ? volumeControl.muted ? qsTr("Muted")
                                  : qsTr("Volume at %1%").arg(Math.round(volumeControl.getVolumePercentage()))
            : ""
    visible: volumeControl.visible || mpris2.players.length > 0

    component: ColumnLayout {
        spacing: 0

        Subheader {
            text: qsTr("Volume")
            visible: volumeControl.visible
        }

        SoundIndicator.VolumeSlider {
            id: volumeSlider
            visible: volumeControl.visible
        }

        Subheader {
            text: qsTr("Music")
            visible: mpris2.players.length > 0
        }

        Repeater {
            model: mpris2.players

            SoundIndicator.MprisItem {
                player: modelData
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    SoundIndicator.VolumeControl {
        id: volumeControl
    }

    Mpris {
        id: mpris2
    }
}
