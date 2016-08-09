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
import Hawaii.Mpris 1.0
import Hawaii.Settings 1.0 as Settings
import ".."
import "sound" as SoundIndicator

Indicator {
    id: indicator
    name: "sound"
    iconName: volumeControl.getIconName()
    component: Component {
        ColumnLayout {
            spacing: Units.largeSpacing

            HeadlineLabel {
                text: qsTr("Sound")
            }

            SubheadingLabel {
                text: qsTr("Volume")
                visible: volumeControl.visible
            }

            SoundIndicator.VolumeSlider {
                id: volumeSlider
                visible: volumeControl.visible

                Layout.fillWidth: true
            }

            SubheadingLabel {
                text: qsTr("Playback")
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
    visible: volumeControl.visible || mpris2.players.length > 0

    SoundIndicator.VolumeControl {
        id: volumeControl
    }

    Mpris {
        id: mpris2
    }

    Settings.Settings {
        id: mmKeybindings
        schema.id: "org.hawaiios.desktop.keybindings.multimedia"
        schema.path: "/org/hawaiios/desktop/keybindings/multimedia/"
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: mmKeybindings.volumeMute
        onActivated: volumeControl.toggleMute()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: mmKeybindings.volumeUp
        onActivated: volumeControl.increase()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: mmKeybindings.volumeDown
        onActivated: volumeControl.decrease()
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
