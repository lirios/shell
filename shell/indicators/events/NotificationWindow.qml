/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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
import QtQuick.Controls.Material 2.0
import Hawaii.Themes 1.0 as Themes
import Fluid.Controls 1.0
import "../../components" as ShellComponents

Item {
    property var notificationData
    readonly property alias closing: __priv.closing

    signal expired(Item notificationWindow)
    signal closed(Item notificationWindow)
    signal actionInvoked(string actionId)

    id: root
    objectName: "notificationWindow"
    width: Math.round(Units.gu(24))
    height: notificationItem.implicitHeight
    opacity: 0.0
    onOpacityChanged: {
        if (opacity == 0.0 && __priv.closing) {
            // Destroy notification
            root.parent = null;
            root.destroy();
        }
    }

    Material.theme: Material.Dark

    Behavior on y {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Units.longDuration
        }
    }

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Units.longDuration
        }
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.OutSine
            duration: Units.longDuration
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
            topMargin: -Units.smallSpacing * 1.5
            rightMargin: -Units.smallSpacing * 1.5
        }
        z: 1
        onClicked: root.closed(root)
    }

    Rectangle {
        id: bubble
        anchors.fill: parent
        color: Material.dialogColor
        gradient: Gradient {
            GradientStop { position: 0; color: Qt.lighter(Material.dialogColor, 1.2) }
            GradientStop { position: 1; color: Qt.darker(Material.dialogColor, 1.1) }
        }
        border.width: Units.gu(0.05)
        border.color: Themes.Theme.palette.rgba(Qt.darker(Material.drawerBackgroundColor, 1.2), 0.5)
        radius: Units.gu(0.4)
        antialiasing: true
        z: 0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutSine
                duration: Units.longDuration
            }
        }

        NotificationItem {
            id: notificationItem
            anchors {
                fill: parent
                margins: Units.smallSpacing
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
