/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Fluid.Controls 1.0
import Liri.Notifications 1.0

ListView {
    id: listView
    spacing: Units.largeSpacing
    interactive: false
    model: ListModel {
        id: notificationsModel
    }
    verticalLayoutDirection: ListView.BottomToTop
    delegate: NotificationDelegate {
        onExpired: removeNotification(model.id)
        onClosed: removeNotification(model.id)
    }
    add: Transition {
        NumberAnimation {
            properties: "x,y"
            easing.type: Easing.OutQuad
            duration: Units.longDuration
        }
    }
    remove: Transition {
        NumberAnimation {
            properties: "x,y"
            easing.type: Easing.OutQuad
            duration: Units.longDuration
        }
    }
    populate: Transition {
        NumberAnimation {
            properties: "x,y"
            easing.type: Easing.OutQuad
            duration: Units.longDuration
        }
    }
    displaced: Transition {
        NumberAnimation {
            properties: "x,y"
            easing.type: Easing.OutQuad
            duration: Units.longDuration
        }
    }

    Connections {
        target: NotificationsService
        onNotificationReceived: {
            notificationsModel.append({"id": notificationId, "appName": appName,
                                          "appIcon": appIcon, "hasIcon": hasIcon,
                                          "summary": summary, "body": body,
                                          "actions": actions, "isPersistent": isPersistent,
                                          "expireTimeout": expireTimeout, "hints": hints})
        }
    }

    function removeNotification(id) {
        for (var i = notificationsModel.count - 1; i >=0; --i) {
            if (notificationsModel.get(i).id === id) {
                notificationsModel.remove(i)
                break
            }
        }
    }
}
