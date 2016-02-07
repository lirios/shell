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

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Private 1.0 as ControlsPrivate
import Hawaii.Components 1.0 as Components
import "screens" as Screens

Components.NoiseBackground {
    id: root
    color: "#272727"
    gradient: Gradient {
        GradientStop { position: 0; color: "#272727" }
        GradientStop { position: 1; color: "#2b2b2b" }
    }

    Connections {
        target: sddm
        onLoginSucceeded: stackView.push({"item": desktopStillComponent})
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: Qt.ArrowCursor
    }

    Component {
        id: bootSplashComponent

        Screens.BootSplash {}
    }

    Component {
        id: greeterComponent

        Screens.Greeter {
            rebootVisible: sddm.canReboot
            powerOffVisible: sddm.canPowerOff
            onLoginRequested: sddm.login(userName, password, sessionIndex)
            onRebootRequested: sddm.reboot()
            onPowerOffRequested: sddm.powerOff()
        }
    }

    Component {
        id: emptyComponent

        Item {}
    }

    Component {
        id: desktopStillComponent

        Screens.DesktopStill {}
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: bootSplashComponent
        delegate: ControlsPrivate.StackViewSlideDelegate {
            horizontal: false
        }
    }

    Timer {
        interval: 2000
        running: true
        onTriggered: stackView.push({"item": primaryScreen ? greeterComponent : emptyComponent})
    }
}
