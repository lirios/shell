/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Themes 1.0 as Themes
import "../../components" as ShellComponents

Item {
    property var notificationData
    readonly property alias closing: __priv.closing

    signal expired(Item notificationWindow)
    signal closed(Item notificationWindow)
    signal actionInvoked(string actionId)

    id: root
    objectName: "notificationWindow"
    width: Math.round(Themes.Units.gu(24))
    height: notificationItem.implicitHeight
    opacity: 0.0
    onOpacityChanged: {
        if (opacity == 0.0 && __priv.closing) {
            // Destroy notification
            root.parent = null;
            root.destroy();
        }
    }

    Behavior on y {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.longDuration
        }
    }

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.longDuration
        }
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.OutSine
            duration: Themes.Units.longDuration
        }
    }

    QtObject {
        id: __priv

        property bool closing: false
    }

    Timer {
        id: timer
        onTriggered: {
            if (!notificationData.isPersistent) {
                timer.running = false;
                root.expired(root);
            }
        }
    }

    ShellComponents.CloseButton {
        anchors {
            top: parent.top
            right: parent.right
            topMargin: -Themes.Units.smallSpacing * 1.5
            rightMargin: -Themes.Units.smallSpacing * 1.5
        }
        z: 1
        onClicked: root.closed(root)
    }

    Rectangle {
        id: bubble
        anchors.fill: parent
        color: Themes.Theme.palette.panel.backgroundColor
        gradient: Gradient {
            GradientStop { position: 0; color: Qt.lighter(Themes.Theme.palette.panel.backgroundColor, 1.2) }
            GradientStop { position: 1; color: Qt.darker(Themes.Theme.palette.panel.backgroundColor, 1.1) }
        }
        border.width: Themes.Units.gu(0.05)
        border.color: Themes.Theme.palette.rgba(Qt.darker(Themes.Theme.palette.panel.backgroundColor, 1.2), 0.5)
        radius: Themes.Units.gu(0.4)
        antialiasing: true
        z: 0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutSine
                duration: Themes.Units.longDuration
            }
        }

        NotificationItem {
            id: notificationItem
            anchors {
                fill: parent
                margins: Themes.Units.smallSpacing
            }
            onActionInvoked: root.actionInvoked(actionId)
        }
    }

    // Do not capture click events, just change opacity when the pointer
    // is moved. This will make stuff underneath visible and clickable
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onEntered: bubble.opacity = 0.5
        onExited: bubble.opacity = 1.0
    }

    function show() {
        opacity = 1.0;
    }

    function close() {
        __priv.closing = true;
        opacity = 0.0;
    }

    function populateNotification(notification) {
        root.notificationData = notification;
        notificationItem.summary = notification.summary;
        notificationItem.body = notification.body;
        notificationItem.hasIcon = notification.hasIcon;
        notificationItem.icon = "image://notifications/%1/%2".arg(notification.id).arg(Date.now() / 1000 | 0);
        timer.interval = notification.expireTimeout;
        timer.restart();
        notificationItem.actions.clear();
        notificationItem.actions.append(notification.actions);
    }
}
