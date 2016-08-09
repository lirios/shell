/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Layouts 1.0
import Fluid.Controls 1.0

Rectangle {
    property alias title: titleLabel.text
    property bool active: false

    signal clicked()
    signal moving(int x, int y)
    signal close()
    signal minimize()
    signal maximize()

    id: root
    gradient: Gradient {
        GradientStop { color: Material.dialogColor; position: 0 }
        GradientStop { color: Material.drawerBackgroundColor; position: 1 }
    }

    MouseArea {
        property bool moving: false

        id: mouseArea
        anchors.fill: parent
        onPressAndHold: moving = true
        onReleased: moving = false
        onClicked: if (!moving) root.clicked()
        onPositionChanged: root.moving(mouse.x, mouse.y)
    }

    RowLayout {
        anchors {
            fill: parent
            margins: Units.gu(0.25)
        }
        spacing: Units.gridUnit
        opacity: root.active ? 1.0 : 0.5

        WindowControls {
            id: controls
            height: parent.height
            onClose: root.close();
            onMinimize: root.minimize();
            onMaximize: root.maximize();

            Layout.alignment: Qt.AlignVCenter
        }

        Label {
            id: titleLabel
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            style: Text.Raised
            styleColor: Material.secondaryTextColor
            height: parent.height
            renderType: Text.NativeRendering

            Layout.fillHeight: true
        }
    }
}
