/****************************************************************************
 * This file is part of Kahai.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import ".."

Indicator {
    property int notificationId: 0
    property var pendingRemovals: []

    name: "events"
    iconName: "dialog-information-symbolic"
    component: Component {
        ColumnLayout {
            spacing: units.largeSpacing

            PlasmaExtras.Heading {
                text: qsTr("Events")
                color: Theme.panel.textColor
            }

            ListView {
                id: notificationView
                spacing: units.largeSpacing
                clip: true
                model: notificationsModel
                delegate: EventItem {}
                add: Transition {
                    NumberAnimation {
                        properties: "x"
                        from: notificationView.width
                        duration: units.shortDuration
                    }
                }
                remove: Transition {
                    NumberAnimation {
                        properties: "x"
                        to: notificationView.width
                        duration: units.longDuration
                    }

                    NumberAnimation {
                        properties: "opacity"
                        to: 0
                        duration: units.longDuration
                    }
                }
                removeDisplaced: Transition {
                    SequentialAnimation {
                        PauseAnimation {
                            duration: units.longDuration
                        }

                        NumberAnimation {
                            properties: "x,y"
                            duration: units.shortDuration
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

    PlasmaCore.DataSource {
        id: notificationsSource
        engine: "notifications"
        interval: 0
        onSourceAdded: connectSource(source)
        onSourceRemoved: {
            var i;
            for (i = 0; i < notificationsModel.count; ++i) {
                if (notificationsModel.get(i) === source) {
                    notificationsModel.remove(i);
                    break;
                }
            }
        }
        onNewData: {
            var actions = new Array();

            if (data["actions"] && data["actions"].length % 2 == 0) {
                var i;
                for (i = 0; i < data["actions"].length; i += 2) {
                    var action = new Object();
                    action["id"] = data["actions"][i];
                    action["text"] = data["actions"][i + 1];
                    actions.push(action);
                }
            }

            addNotification(sourceName, data, actions);
        }
    }

    ListModel {
        id: notificationsModel
    }

    /*
        Notification data object has the following properties:
         - appIcon
         - image
         - appName
         - summary
         - body
         - isPersistent
         - expireTimeout
         - urgency
         - appRealName
         - configurable
    */
    function addNotification(source, data, actions) {
        // Do not show duplicated notifications
        // Remove notifications that are sent again (odd, but true)
        var i;
        for (i = 0; i < notificationsModel.count; ++i) {
            var tmp = notificationsModel.get(i);
            var matches = (tmp.appName === data.appName &&
                           tmp.summary === data.summary &&
                           tmp.body === data.body);
            var sameSource = tmp.source === source;

            if (sameSource && matches)
                return;

            if (sameSource || matches) {
                notificationsModel.remove(i)
                break;
            }
        }

        if (!sameSource && !matches)
            badgeCount++;

        data["id"] = ++notificationId;
        data["source"] = source;
        if (data["summary"].length < 1) {
            data["summary"] = data["body"];
            data["body"] = '';
        }
        data["actions"] = actions;

        notificationsModel.insert(0, data);
        if (!data["isPersistent"]) {
            pendingRemovals.push(notificationId);
            pendingTimer.start();
        }
    }
}
