/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2014 Aaron Seigo <aseigo@kde.org>
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes
import org.hawaii.notifications 0.1
import ".."
import "events" as EventsIndicator

Indicator {
    property int notificationId: 0
    property var pendingRemovals: []

    name: "events"
    iconName: "dialog-information-symbolic"
    component: Component {
        ColumnLayout {
            spacing: Themes.Units.largeSpacing

            Controls.Heading {
                text: qsTr("Events")
                color: Themes.Theme.palette.panel.textColor
            }

            Label {
                color: Themes.Theme.palette.panel.textColor
                opacity: 0.6
                text: qsTr("No new events to see.")
                visible: notificationView.count == 0
            }

            ListView {
                id: notificationView
                spacing: Themes.Units.largeSpacing
                clip: true
                model: notificationsModel
                delegate: EventsIndicator.EventItem {}
                add: Transition {
                    NumberAnimation {
                        properties: "x"
                        from: notificationView.width
                        duration: Themes.Units.shortDuration
                    }
                }
                remove: Transition {
                    NumberAnimation {
                        properties: "x"
                        to: notificationView.width
                        duration: Themes.Units.longDuration
                    }

                    NumberAnimation {
                        properties: "opacity"
                        to: 0
                        duration: Themes.Units.longDuration
                    }
                }
                removeDisplaced: Transition {
                    SequentialAnimation {
                        PauseAnimation {
                            duration: Themes.Units.longDuration
                        }

                        NumberAnimation {
                            properties: "x,y"
                            duration: Themes.Units.shortDuration
                        }
                    }
                }

                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
    onTriggered: badgeCount = 0

    Timer {
        id: pendingTimer
        interval: 5000
        repeat: false
        onTriggered: {
            var i;
            for (i = 0; i < pendingRemovals.length; ++i) {
                var id = pendingRemovals[i];
                var j;
                for (j = 0; j < notificationsModel.count; ++j) {
                    if (notificationsModel.get(j) == id)
                        notificationsModel.remove(j);
                }
            }

            pendingRemovals = [];
        }
    }

    Notifications {
        id: notificationsService
        onNotificationReceived: addNotification(data)
        onNotificationClosed: repositionNotifications()
    }

    ListModel {
        id: notificationsModel
    }

    Component.onCompleted: {
        // Move notifications every time the available geometry changes
        //_greenisland_output.availableGeometryChanged.connect(repositionNotifications);
    }

    /*
        Notification data object has the following properties:
         - id
         - appName
         - appIcon
         - image
         - summary
         - body
         - actions
         - isPersistent
         - expireTimeout
         - hints
    */
    function addNotification(data) {
        // Do not show duplicated notifications
        // Remove notifications that are sent again (odd, but true)
        var i;
        for (i = 0; i < notificationsModel.count; ++i) {
            var tmp = notificationsModel.get(i);
            var matches = (tmp.appName === data.appName &&
                           tmp.summary === data.summary &&
                           tmp.body === data.body);
            var sameSource = tmp.id === data["id"];

            if (sameSource && matches)
                return;

            if (sameSource || matches) {
                notificationsModel.remove(i)
                break;
            }
        }

        if (!sameSource && !matches)
            badgeCount++;

        if (data["summary"].length < 1) {
            data["summary"] = data["body"];
            data["body"] = "";
        }

        notificationsModel.insert(0, data);
        if (!data["isPersistent"]) {
            pendingRemovals.push(data["id"]);
            pendingTimer.start();
        }

        // Print notification data
        console.debug(JSON.stringify(data));

        // Update notification window if it's the same source
        // otherwise create a new window
        if (sameSource) {
            var i, found = false;
            var popups = compositorRoot.screenView.layers.notifications.children;
            for (i = 0; i < popups.length; i++) {
                var popup = popups[i];
                if (popup.objectName !== "notificationWindow")
                    continue;

                if (popup.notificationData.id === data["id"]) {
                    popup.populateNotification(data);
                    found = true;
                    break;
                }
            }

            if (!found)
                createNotificationWindow(data);
        } else {
            createNotificationWindow(data);
        }
    }

    function createNotificationWindow(data) {
        var component = Qt.createComponent("events/NotificationWindow.qml");
        if (component.status !== Component.Ready) {
            console.error(component.errorString());
            return;
        }

        var window = component.createObject(compositorRoot.screenView.layers.notifications);
        window.heightChanged.connect(repositionNotifications);
        window.actionInvoked.connect(function(actionId) {
            notificationsService.invokeAction(data["id"], actionId);
        });
        window.closed.connect(function(notificationWindow) {
            notificationsService.closeNotification(data["id"], Notifications.CloseReasonByUser);
            notificationWindow.close();
            repositionNotifications();
        });
        window.expired.connect(function(notificationWindow) {
            notificationsService.closeNotification(data["id"], Notifications.CloseReasonExpired);
            notificationWindow.close();
            repositionNotifications();
        });
        window.populateNotification(data);
        repositionNotifications();
        window.show();
    }

    function repositionNotifications() {
        var popups = compositorRoot.screenView.layers.notifications.children;
        var workArea = _greenisland_output.availableGeometry;
        var offset = Themes.Theme.mSize().height;
        var totalHeight = 0;

        var i;
        for (i = 0; i < popups.length; i++) {
            var popup = popups[i];
            if (popup.objectName !== "notificationWindow")
                continue;

            // Ignore notification windows that are about to be destroyed
            if (popup.closing)
                continue;

            popup.x = workArea.width - popup.width - offset;
            popup.y = workArea.height - totalHeight - popup.height;
            totalHeight += popup.height + offset;
        }
    }
}
