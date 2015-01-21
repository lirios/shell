/****************************************************************************
 * This file is part of Hawaii.
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

    Rectangle {
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

        ShellComponents.CloseButton {
            anchors {
                top: parent.top
                right: parent.right
                topMargin: -Themes.Units.smallSpacing * 1.5
                rightMargin: -Themes.Units.smallSpacing * 1.5
            }
            onClicked: root.closed(root)
        }

        NotificationItem {
            id: notificationItem
            anchors {
                fill: parent
                margins: Themes.Units.smallSpacing
            }
            summary: notificationData ? notificationData.summary : ""
            body: notificationData ? notificationData.body : ""
            icon: notificationData ? notificationData.appIcon : ""
            hasIcon: notificationData ? notificationData.appIcon !== "" : false
            image: notificationData ? notificationData.image : undefined
            hasImage: notificationData ? notificationData.image !== undefined : false
            onActionInvoked: root.actionInvoked(actionId)
        }
    }

    // Do not capture click events, just change opacity when the pointer
    // is moved. This will make stuff underneath visible and clickable
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onEntered: root.opacity = 0.5
        onExited: root.opacity = 1.0
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
        timer.interval = notification.expireTimeout;
        timer.restart();
        notificationItem.actions.clear();
        notificationItem.actions.append(notification.actions);
    }
}
