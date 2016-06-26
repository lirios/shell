/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Ui 1.0 as FluidUi

Rectangle {
    property alias iconSize: icon.width
    property bool hovered: false

    color: hovered ? Material.accent : "transparent"
    radius: FluidUi.Units.dp(6)
    antialiasing: true

    Image {
        id: icon
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            leftMargin: FluidUi.Units.smallSpacing
            topMargin: FluidUi.Units.smallSpacing
            rightMargin: FluidUi.Units.smallSpacing
        }
        height: width
        sourceSize.width: width
        sourceSize.height: height
        source: "image://launcher/" + model.iconName
        cache: false
        asynchronous: true
    }

    Label {
        id: label
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: icon.bottom
            bottom: parent.bottom
            leftMargin: FluidUi.Units.smallSpacing
            topMargin: FluidUi.Units.smallSpacing
            rightMargin: FluidUi.Units.smallSpacing
            bottomMargin: FluidUi.Units.smallSpacing
        }
        text: model.name
        elide: Text.ElideRight
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        textFormat: Text.PlainText
        horizontalAlignment: Text.AlignHCenter
        width: iconSize * 1.5
        height: FluidUi.Units.gu(2)
    }
}
