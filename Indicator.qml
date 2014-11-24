/****************************************************************************
 * This file is part of Kahai.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import "."
import "components"

Item {
    property string name
    property alias iconName: icon.iconName
    property alias text: label.text
    property int badgeCount: 0
    property bool active: false
    property string tooltip
    readonly property bool selected: selectedIndicator == indicator
    property Component component

    signal triggered(var caller)

    id: indicator
    width: {
        if (!visible)
            return 0;

        var size = 0;
        if (iconName)
            size += icon.width + (units.smallSpacing * 2);
        if (text)
            size += label.width + (units.smallSpacing * 2);
        if (iconName && text)
            size += units.smallSpacing * 2;
        return Math.max(size, units.smallSpacing * 10);
    }
    height: Math.max(Math.max(icon.height, label.height) + (units.smallSpacing * 2), units.smallSpacing * 10)

    Rectangle {
        id: container
        anchors.fill: parent
        radius: width * 0.5
        color: Theme.panel.selectedBackgroundColor
        opacity: active ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: units.shortDuration
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onClicked: indicator.triggered(indicator)
    }

    Icon {
        id: icon
        anchors.centerIn: parent
        color: selected ? Theme.panel.selectedTextColor : Theme.panel.textColor
        width: units.roundToIconSize(units.iconSizes.smallMedium)
        height: width
    }

    Label {
        id: label
        anchors.centerIn: parent
        color: Theme.panel.textColor
        font.pixelSize: units.roundToIconSize(units.iconSizes.small)
    }

    Rectangle {
        id: badge
        anchors {
            top: parent.top
            right: parent.right
            topMargin: -(units.smallSpacing * 0.5)
            rightMargin: -(units.largeSpacing * 0.5)
        }
        width: units.iconSizes.smallMedium
        height: width
        radius: width * 0.5
        color: "orangered"
        opacity: indicator.badgeCount > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: units.smallDuration
            }
        }

        Label {
            anchors.centerIn: parent
            font.pixelSize: parent.width - units.smallSpacing
            color: "white"
            text: indicator.badgeCount
        }
    }
}
