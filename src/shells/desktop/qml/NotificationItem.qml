/****************************************************************************
 * This file is part of Desktop Shell.
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
import GreenIsland 1.0
import FluidCore 1.0
import FluidUi 1.0

PopupWindow {
    id: notification
    width: defaultWidth
    height: defaultHeight
    visible: false

    property int identifier
    property string appName
    property string iconName
    property alias summary: summaryText.text
    property alias body: bodyText.text
    property variant image
    property int timeout

    property real defaultWidth: 240 + frame.margins.left + frame.margins.right + (padding * 2)
    property real defaultHeight: 50 + frame.margins.top + frame.margins.bottom + (padding * 2)
    property real padding: 10

    property real normalOpacity: 0.8
    property real fadeOpacity: 0.5

    property int mouseOverMargin: 48
    property real mouseOverOpacityMin: 0.4

    signal closed(int identifier)

    onVisibleChanged: console.log("**************************", identifier, visible)

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
        onRunningChanged: console.log("Timeout!!!!!!!!!!!!", identifier)
        onTriggered: {
            console.log("Closing notification", identifier, "...");
            notification.closed(identifier)
        }
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

    FrameSvgItem {
        id: frame
        anchors.fill: parent
        imagePath: "widgets/tooltip"
    }

    Item {
        anchors {
            fill: frame
            leftMargin: frame.margins.left
            topMargin: frame.margins.top
            rightMargin: frame.margins.right
            bottomMargin: frame.margins.bottom
        }

        Image {
            id: iconImage
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: padding
                topMargin: padding
            }
            smooth: true
            sourceSize: Qt.size(theme.largeIconSize, theme.largeIconSize)
            source: iconName
            onSourceChanged: visible = source != ""
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
            onTextChanged: {
                if (text == "") {
                    visible = false;
                    summaryText.anchors.bottom = parent.bottom;
                    anchors.bottom = undefined;
                    height = defaultHeight;
                } else {
                    visible = true;
                    summaryText.anchors.bottom = undefined;
                    anchors.bottom = parent.bottom;
                    height = defaultHeight + paintedHeight;
                }
            }
        }
    }

    function start() {
        //notification.visible = true;
        timer.running = true;
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
        notification.body += "<br>" + text;
        // TODO: Use timeout
    }

    function timeoutForText(text) {
        var kAverageWordLength = 6;
        var kWordPerMinute = 250;
        return 60000 * text.length / kAverageWordLength / kWordPerMinute;
    }
}
