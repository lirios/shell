/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

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
            size += icon.width + (Themes.Units.smallSpacing * 2);
        if (text)
            size += label.width + (Themes.Units.smallSpacing * 2);
        if (iconName && text)
            size += Themes.Units.smallSpacing * 2;
        return Math.max(size, Themes.Units.smallSpacing * 10);
    }
    height: Math.max(Math.max(icon.height, label.height) + (Themes.Units.smallSpacing * 2), Themes.Units.smallSpacing * 10)
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
                duration: Themes.Units.shortDuration
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onClicked: indicator.triggered(indicator)
    }

    Components.Icon {
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
        font.pixelSize: Themes.Units.roundToIconSize(Themes.Units.iconSizes.small)
    }

    Rectangle {
        id: badge
        anchors {
            top: parent.top
            right: parent.right
            topMargin: -(Themes.Units.smallSpacing * 0.5)
            rightMargin: -(Themes.Units.largeSpacing * 0.5)
        }
        width: Themes.Units.iconSizes.smallMedium
        height: width
        radius: width * 0.5
        color: "orangered"
        opacity: indicator.badgeCount > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.shortDuration
            }
        }

        Label {
            anchors.centerIn: parent
            font.pixelSize: parent.width - Themes.Units.smallSpacing
            color: "white"
            text: indicator.badgeCount
        }
    }
}
