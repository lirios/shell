/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Layouts 1.0
import Hawaii.Themes 1.0 as Themes

Rectangle {
    property alias iconSize: icon.width
    property bool hovered: false

    color: hovered ? Themes.Theme.palette.rgba(Themes.Theme.palette.panel.selectedBackgroundColor, 0.7) : "transparent"
    radius: Themes.Units.dp(6)
    antialiasing: true

    Image {
        id: icon
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            leftMargin: Themes.Units.largeSpacing
            topMargin: Themes.Units.largeSpacing
            rightMargin: Themes.Units.largeSpacing
        }
        height: width
        sourceSize.width: width
        sourceSize.height: height
        source: "image://launcher/" + model.iconName
        cache: false
        asynchronous: true
    }

    Text {
        id: label
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: icon.bottom
            bottom: parent.bottom
            leftMargin: Themes.Units.largeSpacing
            topMargin: Themes.Units.smallSpacing
            rightMargin: Themes.Units.largeSpacing
            bottomMargin: Themes.Units.largeSpacing
        }
        text: model.name
        renderType: Text.NativeRendering
        color: Themes.Theme.palette.panel.textColor
        elide: Text.ElideRight
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        textFormat: Text.PlainText
        horizontalAlignment: Text.AlignHCenter
        width: iconSize * 1.5
        height: Themes.Units.gu(2)
    }
}
