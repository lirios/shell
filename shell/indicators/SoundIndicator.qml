/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
import GreenIsland 1.0 as GreenIsland
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes
import org.hawaiios.mixer 0.1 as MixerService
import org.hawaiios.mpris2 0.1
import ".."
import "sound" as SoundIndicator

Indicator {
    id: indicator
    name: "sound"
    iconName: {
        if (MixerService.Mixer.muted || MixerService.Mixer.master == 0)
            return "audio-volume-muted-symbolic";
        if (MixerService.Mixer.master < 50)
            return "audio-volume-low-symbolic";
        if (MixerService.Mixer.master < 75)
            return "audio-volume-medium-symbolic";
        return "audio-volume-high-symbolic";
    }
    component: Component {
        ColumnLayout {
            spacing: Themes.Units.largeSpacing

            Controls.Heading {
                text: qsTr("Sound")
                color: Themes.Theme.palette.panel.textColor
            }

            Controls.Heading {
                text: qsTr("Volume")
                color: Themes.Theme.palette.panel.textColor
                level: 3
                visible: volumeControl.visible
            }

            SoundIndicator.VolumeControl {
                id: volumeControl
                visible: MixerService.Mixer.available

                Layout.fillWidth: true
            }

            Controls.Heading {
                text: qsTr("Playback")
                color: Themes.Theme.palette.panel.textColor
                level: 3
                visible: mpris2.players.length > 0
            }

            Repeater {
                model: mpris2.players

                SoundIndicator.MprisItem {
                    player: modelData

                    Layout.fillWidth: true
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
    visible: MixerService.Mixer.available || mpris2.players.length > 0

    Mpris2 {
        id: mpris2
    }

    Connections {
        target: MixerService.Mixer
        onMasterChanged: {
            // Screen view is loaded asynchronously and might not be yet available
            if (!compositorRoot.screenView)
                return;

            // Show overlay
            var overlay = compositorRoot.screenView.layers.overlays;
            overlay.iconName = indicator.iconName;
            overlay.value = MixerService.Mixer.master;
            overlay.showProgress = true;
            if (!overlay.visible)
                overlay.show();
        }
    }

    Connections {
        target: GreenIsland.KeyBindings
        onKeyBindingPressed: {
            switch (name) {
            case "volumeUp":
                MixerService.Mixer.increaseMaster();
                break;
            case "volumeDown":
                MixerService.Mixer.decreaseMaster();
                break;
            case "volumeMute":
                MixerService.Mixer.muted = !MixerService.Mixer.muted;
                break;
            case "mediaPlay":
            case "mediaPrevious":
            case "mediaNext":
                break;
            default:
                break;
            }
        }
    }
}
