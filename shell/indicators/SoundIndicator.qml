/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
import org.hawaii.mixer 0.1 as MixerService
import org.hawaii.mpris2 0.1
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
