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

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0

Rectangle {
    id: item

    property alias highlightColor: highlight.color
    property alias highlightOpacity: highlight.opacity
    property bool active: false
    property alias containsMouse: ripple.containsMouse

    signal clicked(var caller)
    signal rightClicked(var caller)

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
            if (mouse.button == Qt.RightButton)
                item.rightClicked(item)
            else
                item.clicked(item)
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
