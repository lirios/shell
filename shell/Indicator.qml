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
import Qt.labs.controls 1.0
import Hawaii.Themes 1.0 as Themes
import Fluid.Ui 1.0 as FluidUi

Item {
    property string name
    property alias iconName: icon.iconName
    property alias iconSize: icon.width
    property alias text: label.text
    property int badgeCount: 0
    property bool active: false
    property string tooltip
    readonly property bool selected: selectedIndicator == indicator
    readonly property bool expanded: __priv.expanded
    property Component component

    signal triggered(var caller)

    id: indicator
    width: {
        if (!visible)
            return 0;

        var size = 0;
        if (iconName)
            size += icon.width + (FluidUi.Units.smallSpacing * 2);
        if (text)
            size += label.width + (FluidUi.Units.smallSpacing * 2);
        if (iconName && text)
            size += FluidUi.Units.smallSpacing * 2;
        return Math.max(size, FluidUi.Units.smallSpacing * 10);
    }
    height: Math.max(Math.max(icon.height, label.height) + (FluidUi.Units.smallSpacing * 2), FluidUi.Units.smallSpacing * 10)
    onTriggered: __priv.expanded = !__priv.expanded

    QtObject {
        id: __priv

        property bool expanded: false
    }

    Rectangle {
        id: container
        anchors.fill: parent
        radius: width * 0.5
        color: Themes.Theme.palette.panel.selectedBackgroundColor
        opacity: active ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: FluidUi.Units.shortDuration
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onClicked: indicator.triggered(indicator)
    }

    FluidUi.Icon {
        id: icon
        anchors.centerIn: parent
        color: selected ? Themes.Theme.palette.panel.selectedTextColor : Themes.Theme.palette.panel.textColor
        height: width
        cache: false
    }

    Label {
        id: label
        anchors.centerIn: parent
        color: Themes.Theme.palette.panel.textColor
        font.pixelSize: FluidUi.Units.roundToIconSize(FluidUi.Units.iconSizes.small)
    }

    Rectangle {
        id: badge
        anchors {
            top: parent.top
            right: parent.right
            topMargin: -(FluidUi.Units.smallSpacing * 0.5)
            rightMargin: -(FluidUi.Units.largeSpacing * 0.5)
        }
        width: FluidUi.Units.iconSizes.smallMedium
        height: width
        radius: width * 0.5
        color: "orangered"
        opacity: indicator.badgeCount > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: FluidUi.Units.shortDuration
            }
        }

        Label {
            anchors.centerIn: parent
            font.pixelSize: parent.width - FluidUi.Units.smallSpacing
            color: "white"
            text: indicator.badgeCount
        }
    }
}
