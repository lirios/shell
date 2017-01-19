/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini
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

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import Fluid.Controls 1.0
import Liri.Shell 1.0
import Vibe.Notifications 1.0

Indicator {
    readonly property bool hasNotifications: notificationView.count > 0
    readonly property bool silentMode: false

    title: qsTr("Notifications")
    iconName: silentMode ? "social/notifications_off"
                         : hasNotifications ? "social/notifications"
                                            : "social/notifications_none"
    component: ColumnLayout {
        spacing: 0

        Placeholder {
            Layout.fillWidth: true
            Layout.fillHeight: true

            iconName: "social/notifications_none"
            text: qsTr("No notifications")
            visible: notificationView.count == 0
        }

        ListView {
            id: notificationView
            spacing: Units.largeSpacing
            clip: true
            model: notificationsModel
            visible: count > 0
            delegate: ListItem {
                property int notificationId: model.id

                iconName: model.appIcon ? model.appIcon : model.hasIcon ? "image://notifications/%1/%2".arg(model.id).arg(Date.now() / 1000 | 0) : "social/notifications"
                text: model.summary
                onClicked: {
                    for (var i = 0; i < notificationsModel.count; i++) {
                        var notification = notificationsModel.get(i)
                        if (notification.id === notificationId) {
                            notificationsModel.remove(i)
                            break
                        }
                    }
                }
            }
            add: Transition {
                NumberAnimation {
                    properties: "x"
                    from: notificationView.width
                    duration: Units.shortDuration
                }
            }
            remove: Transition {
                NumberAnimation {
                    properties: "x"
                    to: notificationView.width
                    duration: Units.longDuration
                }

                NumberAnimation {
                    properties: "opacity"
                    to: 0
                    duration: Units.longDuration
                }
            }
            removeDisplaced: Transition {
                SequentialAnimation {
                    PauseAnimation {
                        duration: Units.longDuration
                    }

                    NumberAnimation {
                        properties: "x,y"
                        duration: Units.shortDuration
                    }
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
    onClicked: badgeCount = 0

    ListModel {
        id: notificationsModel
    }

    Connections {
        target: NotificationsService
        onNotificationReceived: {
            // Replace existing notification
            for (var i = 0; i < notificationsModel.count; i++) {
                var notification = notificationsModel.get(i)
                if (notification.id === notificationId) {
                    notification.summary = summary
                    notification.body = body
                    notification.actions = actions
                    notification.hints = hints
                    return
                }
            }

            // Add new notifications
            notificationsModel.append({"id": notificationId, "appName": appName,
                                          "appIcon": appIcon, "hasIcon": hasIcon,
                                          "summary": summary, "body": body,
                                          "actions": actions, "isPersistent": isPersistent,
                                          "expireTimeout": expireTimeout, "hints": hints})
            badgeCount++
        }
    }
}
