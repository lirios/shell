/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import Hawaii.Shell 1.0
import Hawaii.Shell.Styles 1.0

Item {
    id: notification
    width: root.implicitWidth
    height: root.implicitHeight

    property int identifier
    property string appName
    property alias iconName: root.iconName
    property alias summary: root.summary
    property alias body: root.body
    property int expirationTimeout
    property int timeout: {
        // If no timeout is specified, add two seconds to ensure the
        // notification last at least five seconds
        if (expirationTimeout < 0)
            return 2000 + Math.max(timeoutForText(summary + body), 3000);
        return expirationTimeout;
    }
    property alias running: timer.running

    signal closed(int identifier)

    Timer {
        id: timer
        interval: timeout
        repeat: false
        onTriggered: notification.closed(identifier)
    }

    Behavior on height {
        NumberAnimation { easing.type: Easing.InQuad }
    }

    Behavior on opacity {
        NumberAnimation { easing.type: Easing.InOutQuad; duration: 250 }
    }

    StyledItem {
        property string iconName
        property string summary
        property string body

        id: root
        style: Qt.createComponent(StyleSettings.path + "/NotificationBubbleStyle.qml", root)

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            onEntered: notification.opacity = 0.5
            onExited: notification.opacity = 1.0
        }
    }

    function appendToBody(text, timeout) {
        // Append body, this will increase height automatically
        notification.body += "<br>" + text;

        // Reset timer
        notification.timeout = Math.max(timeout, timeoutForText(text));
        timer.interval = notification.timeout;
        timer.restart();
    }

    function timeoutForText(text) {
        var kAverageWordLength = 6;
        var kWordPerMinute = 250;
        return 60000 * text.length / kAverageWordLength / kWordPerMinute;
    }
}
