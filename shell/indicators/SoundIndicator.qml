/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.5
import QtQuick.Layouts 1.0
import Hawaii.Controls 1.0 as Controls
import Fluid.Ui 1.0 as FluidUi
import org.hawaiios.mixer 0.1 as MixerService
import org.hawaiios.mpris2 0.1
import Hawaii.GSettings 1.0 as Settings
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
            spacing: FluidUi.Units.largeSpacing

            Controls.Heading {
                text: qsTr("Sound")
            }

            Controls.Heading {
                text: qsTr("Volume")
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
                level: 3
                visible: mpris2.players.length > 0
            }

            Repeater {
                model: mpris2.players

                SoundIndicator.MprisItem {
                    player: modelData

                    Layout.alignment: Qt.AlignHCenter
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
            // Show overlay
            var overlay = screenView.layers.overlays;
            overlay.iconName = indicator.iconName;
            overlay.value = MixerService.Mixer.master;
            overlay.showProgress = true;
            if (!overlay.visible)
                overlay.show();
        }
    }

    Settings.Settings {
        id: mmKeybindings
        schema.id: "org.hawaiios.desktop.keybindings.multimedia"
        schema.path: "/org/hawaiios/desktop/keybindings/multimedia/"
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: mmKeybindings.volumeMute
        onActivated: MixerService.Mixer.muted = !MixerService.Mixer.muted
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: mmKeybindings.volumeUp
        onActivated: MixerService.Mixer.increaseMaster()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: mmKeybindings.volumeDown
        onActivated: MixerService.Mixer.decreaseMaster()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: mmKeybindings.mediaPlay
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: mmKeybindings.mediaPrevious
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: mmKeybindings.mediaNext
    }
}
