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

import QtQuick 2.5
import QtQuick.Layouts 1.0
import Fluid.Controls 1.0 as FluidControls
import Liri.Shell 1.0
import Vibe.Mpris 1.0
import QtGSettings 1.0 as Settings
import "sound" as SoundIndicator

Indicator {
    id: indicator

    title: qsTr("Sound")
    iconSource: FluidControls.Utils.iconUrl(volumeControl.getIconName())
    tooltip: volumeControl.visible
            ? volumeControl.muted ? qsTr("Muted")
                                  : qsTr("Volume at %1%").arg(Math.round(volumeControl.getVolumePercentage()))
            : ""
    visible: volumeControl.visible || mpris2.players.length > 0

    component: ColumnLayout {
        spacing: 0

        FluidControls.Subheader {
            text: qsTr("Volume")
            visible: volumeControl.visible
        }

        SoundIndicator.VolumeSlider {
            id: volumeSlider
            visible: volumeControl.visible
        }

        FluidControls.Subheader {
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

    Mpris {
        id: mpris2
    }
}
