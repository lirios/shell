/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Shell.Desktop 0.1
import Hawaii.Shell.Styles 0.1

NotificationWindow {
    id: notification
    color: "transparent"
    width: root.implicitWidth
    height: root.implicitHeight

    property int identifier
    property string appName
    property alias iconName: root.iconName
    property alias summary: root.summary
    property alias body: root.body
    property alias picture: root.picture
    property int timeout

    property real normalOpacity: 0.8
    property real fadeOpacity: 0.5

    property int mouseOverMargin: 48
    property real mouseOverOpacityMin: 0.4

    signal closed(int identifier)

    onTimeoutChanged: {
        // If no timeout is specified, add two seconds to ensure the
        // notification last at least five seconds
        if (timeout < 0)
            timeout = 2000 + Math.max(timeoutForText(summary + body), 3000);
    }

    Timer {
        id: timer
        interval: timeout
        running: visible
        repeat: false
        onTriggered: notification.closed(identifier)
    }

    Behavior on height {
        NumberAnimation { easing.type: Easing.InQuad }
    }

    /*
    NumberAnimation {
        id: mouseOverAnimation
        target: container
        properties: "opacity"
        duration: 500
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: setOpacityFromPosition(mouseX, mouseY)
        onExited: {
            mouseOverAnimation.to = notification.normalOpacity;
            mouseOverAnimation.start();
        }
    }
    */

    StyledItem {
        property string iconName
        property string summary
        property string body
        property var picture

        id: root
        style: Qt.createComponent("NotificationBubbleStyle.qml", root)
    }

    function distance(value, min, max) {
        if (value <= min)
            return min - value;
        if (value >= max)
            return value - max;
        return 0;
    }

    function opacityFromPosition(mouseX, mouseY) {
        var distanceX = distance(mouseX, left, right);
        if (distanceX >= mouseOverMargin)
            return normalOpacity;

        var distanceY = distance(mouseY, top, bottom);
        if (distanceY >= mouseOverMargin)
            return normalOpacity;

        var d = Math.sqrt(distanceX * distanceX + distanceY * distanceY);
        if (d >= mouseOverMargin)
            return normalOpacity;

        return mouseOverOpacityMin + (normalOpacity - mouseOverOpacityMin) * d / mouseOverMargin;
    }

    function setOpacityFromPosition(mouseX, mouseY) {
        mouseOverAnimation.to = opacityFromPosition(mouseX, mouseY);
        mouseOverAnimation.start();
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
