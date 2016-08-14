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
import QtQuick.Controls 2.0
import Fluid.Controls 1.0
import "screens" as Screens

NoiseBackground {
    id: root
    width: 1024
    height: 768
    color: "#272727"
    gradient: Gradient {
        GradientStop { position: 0; color: "#272727" }
        GradientStop { position: 1; color: "#2b2b2b" }
    }

    Connections {
        target: sddm
        onLoginSucceeded: stackView.replace(desktopStillComponent)
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
        replaceEnter: Transition {
            YAnimator {
                from: height
                to: 0
            }
        }
        replaceExit: Transition {
            YAnimator {
                from: 0
                to: -height
            }
        }
    }

    Timer {
        interval: 2 // 2000
        running: true
        //onTriggered: stackView.replace(primaryScreen ? greeterComponent : emptyComponent)
        onTriggered: stackView.replace(greeterComponent)
    }
}
