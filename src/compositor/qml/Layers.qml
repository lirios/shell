/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

Item {
    property alias lock: lockLayer
    property alias overlay: overlayLayer
    property alias dialogs: dialogsLayer
    property alias fullScreen: fullScreenLayer
    property alias panels: panelsLayer
    property alias notifications: notificationsLayer
    property alias windows: windowsLayer
    property alias desktop: desktopLayer
    property alias background: backgroundLayer

    // Lock screen is above all windows to shield the session
    Item {
        id: lockLayer
        anchors.fill: parent
        z: 9
    }

    // Overlays can cover pretty much everything except the lock screen
    Item {
        id: overlayLayer
        anchors.fill: parent
        z: 8
    }

    // Globally modal dialogs can cover application and shell gadgets
    Item {
        id: dialogsLayerContainer
        anchors.fill: parent
        z: 7

        // Overlay
        Rectangle {
            id: modalOverlay
            anchors.fill: parent
            color: "black"
            opacity: 0.0

            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                    duration: 250
                }
            }
        }

        // Actual globally modal dialogs layer
        Item {
            property alias overlay: modalOverlay

            id: dialogsLayer
            anchors.fill: modalOverlay
        }
    }

    // Full screen windows can cover application windows and panels
    Item {
        id: fullScreenLayer
        anchors.fill: parent
        z: 6
    }

    // Panels are above application windows
    Item {
        id: panelsLayer
        anchors.fill: parent
        z: 5
    }

    // Notifications are basically at the same level as panels
    Item {
        id: notificationsLayer
        anchors.fill: parent
        z: 4
    }

    // Application windows can only cover the desktop
    Item {
        id: windowsLayer
        anchors.fill: parent
        z: 3
    }

    // Desktop is only above to the background
    Item {
        id: desktopLayer
        anchors.fill: parent
        z: 2
    }

    // Background is below everything
    Item {
        id: backgroundLayer
        anchors.fill: parent
        z: 1
    }
}
