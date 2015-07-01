/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Layouts 1.0
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

Item {
    property alias text: label.text
    property alias iconName: icon.iconName
    property alias hover: mouseArea.hover

    signal clicked()

    id: root
    width: layout.implicitWidth + (Themes.Units.smallSpacing * 4)
    height: layout.implicitHeight + (Themes.Units.smallSpacing * 4)

    Rectangle {
        id: container
        anchors {
            left: parent.left
            top: parent.top
            margins: Themes.Units.smallSpacing
        }
        gradient: Gradient {
            GradientStop { position: 0; color: Themes.Theme.palette.panel.selectedBackgroundColor }
            GradientStop { position: 1; color: Qt.darker(Themes.Theme.palette.panel.selectedBackgroundColor, 1.45) }
        }
        width: Math.max(layout.implicitWidth + (Themes.Units.smallSpacing * 2), root.parent.width - (Themes.Units.smallSpacing * 2))
        height: layout.implicitHeight + (Themes.Units.smallSpacing * 2)
        radius: Themes.Units.dp(6)
        border.color: Themes.Theme.palette.rgba(Qt.darker(Themes.Theme.palette.panel.selectedBackgroundColor, 1.3), 0.35)
        border.width: Themes.Units.dp(1)
        opacity: hover ? 1.0 : 0.0
        antialiasing: true

        MouseArea {
            property bool hover: false

            id: mouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            hoverEnabled: true
            z: 100
            onEntered: hover = true
            onExited: hover = false
            onClicked: root.clicked()
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.shortDuration
            }
        }
    }

    RowLayout {
        id: layout
        anchors {
            left: container.left
            top: container.top
            margins: Themes.Units.smallSpacing
        }
        spacing: Themes.Units.smallSpacing

        Components.Icon {
            id: icon
            width: Themes.Units.iconSizes.small
            height: width
            color: hover ? Themes.Theme.palette.panel.selectedTextColor : Themes.Theme.palette.panel.textColor
            visible: status == Image.Ready

            Behavior on color {
                ColorAnimation {
                    duration: Themes.Units.shortDuration
                }
            }
        }

        Label {
            id: label
            color: hover ? Themes.Theme.palette.panel.selectedTextColor : Themes.Theme.palette.panel.textColor

            Behavior on color {
                ColorAnimation {
                    duration: Themes.Units.shortDuration
                }
            }

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }
    }
}
