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
import Hawaii.Themes 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import "../../components" as ShellComponents

Rectangle {
    property var notificationData

    signal expired(Item notificationWindow)
    signal closed(Item notificationWindow)
    signal actionInvoked(string actionId)

    id: root
    objectName: "notificationWindow"
    color: Theme.palette.panel.backgroundColor
    gradient: Gradient {
        GradientStop { position: 0; color: Qt.lighter(Theme.palette.panel.backgroundColor, 1.2) }
        GradientStop { position: 1; color: Qt.darker(Theme.palette.panel.backgroundColor, 1.1) }
    }
    border.color: Theme.palette.rgba(Qt.darker(Theme.palette.panel.backgroundColor, 1.2), 0.5)
    width: Math.round(units.gridUnit * 24)
    height: notificationItem.implicitHeight
    radius: units.gridUnit * 0.2
    visible: false

    Behavior on y {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: units.longDuration
        }
    }

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: units.longDuration
        }
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.OutSine
            duration: units.longDuration
        }
    }

    Timer {
        id: timer
        running: root.visible
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
            topMargin: -units.smallSpacing * 1.5
            rightMargin: -units.smallSpacing * 1.5
        }
        onClicked: root.closed(root)
    }

    NotificationItem {
        id: notificationItem
        anchors {
            left: parent.left
            top: parent.top
            margins: units.smallSpacing
        }
        summary: notificationData ? notificationData.summary : ""
        body: notificationData ? notificationData.body : ""
        icon: notificationData ? notificationData.appIcon : ""
        hasIcon: notificationData ? notificationData.appIcon !== "" : false
        image: notificationData ? notificationData.image : undefined
        hasImage: notificationData ? notificationData.image !== undefined : false
        onActionInvoked: root.actionInvoked(actionId)
    }

    function populateNotification(notification) {
        root.notificationData = notification;
        timer.interval = notification.expireTimeout;
        timer.restart();
        notificationItem.actions.clear();
        notificationItem.actions.append(notification.actions);
    }
}
