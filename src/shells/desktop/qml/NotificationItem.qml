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

import QtQuick 2.1
import QtQuick.Controls 1.0
import GreenIsland 1.0
import Hawaii.Shell.Desktop 0.1
import FluidExtra 0.2 as FluidExtra

NotificationWindow {
    id: notification
    color: "transparent"
    width: 240 + (padding * 2) + 2
    height: summaryText.paintedHeight + bodyText.paintedHeight + (padding * 3) + 2

    property int identifier
    property string appName
    property alias iconName: icon.source
    property alias summary: summaryText.text
    property alias body: bodyText.text
    property alias picture: image.image
    property int timeout

    property real padding: 10

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

    Rectangle {
        id: frame
        anchors.fill: parent
        radius: 6
        border.color: "#999"
        color: "white"

        Item {
            id: iconImage
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: padding
                topMargin: padding
            }
            width: 48
            height: 48

            Image {
                id: icon
                anchors.fill: parent
                smooth: true
                sourceSize: Qt.size(48, 48)
                visible: icon.source !== ""
            }

            FluidExtra.ImageItem {
                id: image
                anchors.fill: parent
                smooth: true
                visible: !image.null
            }
        }

        Label {
            id: summaryText
            anchors {
                left: iconImage.right
                top: iconImage.top
                right: parent.right
                leftMargin: padding
            }
            font.weight: Font.Bold
            //color: "white"
            //style: Text.Raised
            //styleColor: "black"
            maximumLineCount: 2
            wrapMode: Text.Wrap
            verticalAlignment: Text.AlignVCenter
        }

        Label {
            id: bodyText
            anchors {
                left: summaryText.left
                top: summaryText.bottom
                right: parent.right
                bottomMargin: padding
            }
            font.pointSize: summaryText.font.pointSize * 0.9
            //color: "white"
            //style: Text.Raised
            //styleColor: "black"
            textFormat: Text.StyledText
            maximumLineCount: 20
            wrapMode: Text.Wrap
            //elide: Text.ElideRight
        }
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
