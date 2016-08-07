/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import Fluid.Material 1.0
import "panel"

PanelItem {
    id: indicator

    property string name

    property alias text: label.text
    property alias iconName: icon.name
    property string tooltip
    property Component iconView

    readonly property int iconSize: height >= 40 ? 56 * 0.36 : height * 0.45

    property int badgeCount: 0

    property Component component

    width: visible ? indicator.text ? label.width + (32 - label.height) : 32 : 0

    Loader {
        id: iconView
        anchors.centerIn: parent
        sourceComponent: indicator.iconView

        property alias iconSize: indicator.iconSize
    }

    Icon {
        id: icon
        size: indicator.iconSize
        anchors.centerIn: parent
    }

    Label {
        id: label
        anchors.centerIn: parent
        font.pixelSize: Units.roundToIconSize(Units.iconSizes.small)
    }

    Rectangle {
        id: badge
        anchors {
            top: parent.top
            right: parent.right
            topMargin: -(Units.smallSpacing * 0.5)
            rightMargin: -(Units.largeSpacing * 0.5)
        }
        width: Units.iconSizes.smallMedium
        height: width
        radius: width * 0.5
        color: "orangered"
        opacity: indicator.badgeCount > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: Units.shortDuration
            }
        }

        Label {
            anchors.centerIn: parent
            font.pixelSize: parent.width - Units.smallSpacing
            color: "white"
            text: indicator.badgeCount
        }
    }
}
