/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Shell.Core 1.0

Indicator {
    id: volumeIndicator
    resources: [
        DataProvider {
            id: volumeControl
            provider: "org.hawaii.dataproviders.mixer"
            connectedSources: ["Master"]
            onDataChanged: setup(data)
        }
    ]

    Slider {
        id: slider
        anchors.fill: parent
        minimumValue: 0
        maximumValue: 100
        onValueChanged: volumeControl.setVolume(value)
    }

    function setup(data) {
        if (data["Master"].volume === 0 || data["Master"].mute) {
            volumeIndicator.iconName = "audio-volume-muted-symbolic";
            return;
        }

        var n = Math.floor(3 * data["Volume"] / 100) + 1;
        if (n < 2)
            volumeIndicator.iconName = "audio-volume-low-symbolic";
        else if (n >= 3)
            volumeIndicator.iconName = "audio-volume-high-symbolic"
        else
            volumeIndicator.iconName = "audio-volume-medium-symbolic";
    }
}
