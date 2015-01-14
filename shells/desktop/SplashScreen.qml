/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.0
import QtGraphicalEffects 1.0
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    property alias progress: bar.value

    id: root

    Image {
        id: picture
        anchors.fill: parent
        source: "/usr/share/backgrounds/gnome/Waves.jpg"
        sourceSize.width: width * 0.75
        sourceSize.height: height * 0.75
        fillMode: Image.PreserveAspectCrop
        cache: false
        visible: false
    }

    FastBlur {
        anchors.fill: picture
        source: picture
        radius: 32
    }

    // Show the progress bar only if we are waiting for a long time
    Timer {
        running: true
        interval: 200
        onTriggered: bar.opacity = 1.0
    }

    ProgressBar {
        id: bar
        anchors.centerIn: parent
        minimumValue: 0.0
        maximumValue: 1.0
        value: 0.0
        opacity: 1.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: units.longDuration
            }
        }
    }
}
