/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.1
import QtQuick.Controls.Private 1.0 as ControlsPrivate

Item {
    property alias usersModel: greeter.usersModel

    id: root

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: Qt.ArrowCursor
    }

    Greeter {
        id: greeter
        width: root.width
        height: root.height
        rebootVisible: sddm.canReboot
        powerOffVisible: sddm.canPowerOff
        onLoginRequested: sddm.login(userName, password, sessionIndex)
        onRebootRequested: sddm.reboot()
        onPowerOffRequested: sddm.powerOff()
    }

    DesktopStill {
        id: desktopStill
        width: root.width
        height: root.height
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: BootSplash {}
        delegate: ControlsPrivate.StackViewSlideDelegate {
            horizontal: false
        }
    }

    Timer {
        id: bootSplashSlideTimer
        interval: 2000
        onTriggered: slideToPageIndex(0)
    }

    Component.onCompleted: bootSplashSlideTimer.start()

    function itemForIndex(index) {
        if (index < 0 || index > 1)
            return null;

        switch (index) {
        case 0:
            return greeter;
        case 1:
            return desktopStill;
        default:
            break;
        }

        return null;
    }

    function slideToPageIndex(index) {
        if (index < 0 || index > 1)
            return;

        var component = itemForIndex(index);
        stackView.push({"item": component});
    }
}
