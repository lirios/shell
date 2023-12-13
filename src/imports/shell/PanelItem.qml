// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Fluid.Controls

Rectangle {
    id: item

    property alias highlightColor: highlight.color
    property alias highlightOpacity: highlight.opacity
    property bool active: false
    property alias containsMouse: ripple.containsMouse

    signal clicked(real x, real y)
    signal rightClicked(real x, real y)

    width: height
    height: parent.height

    color: __priv.highlighted ? Qt.rgba(0,0,0,0.2) : Qt.rgba(0,0,0,0)

    onActiveChanged: __priv.highlighted = active

    Behavior on color {
        ColorAnimation {
            duration: Units.mediumDuration
        }
    }

    QtObject {
        id: __priv

        property bool highlighted
    }

    Ripple {
        id: ripple
        anchors.fill: parent

        color: Qt.rgba(0,0,0,0.3)

        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: {
            if (mouse.button === Qt.RightButton)
                item.rightClicked(mouse.x, mouse.y);
            else
                item.clicked(mouse.x, mouse.y);
        }

        onContainsMouseChanged: {
            if (!item.active)
                __priv.highlighted = containsMouse
        }
    }

    Rectangle {
        id: highlight
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: 2
        color: Material.accentColor

        opacity: active ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: Units.mediumDuration
            }
        }

        Behavior on color {
            ColorAnimation {
                duration: Units.mediumDuration
            }
        }
    }
}
