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
import QtGraphicalEffects 1.0
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import "../../components" as ShellComponents

Item {
    property var window

    signal clicked(var window)

    id: root
    anchors {
        fill: parent
        /*
        leftMargin: window.clientWindow.internalGeometry.x
        topMargin: window.clientWindow.internalGeometry.y
        rightMargin: window.clientWindow.internalGeometry.width
        bottomMargin: window.clientWindow.internalGeometry.height
        */
    }

    RectangularGlow {
        anchors.fill: parent
        glowRadius: 10
        spread: 0.2
        color: Themes.Theme.palette.view.selectedBackgroundColor
        cornerRadius: Themes.Units.gu(0.2)
        opacity: closeButton.opacity > 0.0 ? 0.5 : 0.0
        z: 3

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: Themes.Units.shortDuration
            }
        }
    }

    Rectangle {
        anchors.centerIn: parent
        radius: Themes.Units.dp(6)
        color: "#80000000"
        width: Math.max(parent.width * 0.8, titleLabel.paintedWidth) + Themes.Units.smallSpacing * 2
        height: titleLabel.paintedHeight + Themes.Units.smallSpacing * 2
        z: 4

        Text {
            id: titleLabel
            anchors {
                centerIn: parent
                margins: Themes.Units.smallSpacing
            }
            renderType: Text.NativeRendering
            text: window.clientWindow ? window.clientWindow.title : qsTr("n.a.")
            elide: Text.ElideRight
            color: "white"
        }
    }

    Rectangle {
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: Themes.Units.largeSpacing
            bottomMargin: Themes.Units.largeSpacing
        }
        z: 4

        radius: Themes.Units.dp(6)
        color: "#80000000"
        width: icon.width + Themes.Units.smallSpacing * 2
        height: width

        Components.Icon {
            id: icon
            anchors {
                centerIn: parent
                margins: Themes.Units.smallSpacing
            }
            iconName: window.clientWindow ? window.clientWindow.iconName : ""
            width: Themes.Units.iconSizes.large
            height: width
        }
    }

    ShellComponents.CloseButton {
        id: closeButton
        anchors {
            top: mouseArea.top
            right: mouseArea.right
            margins: -Themes.Units.gu(1)
        }
        width: Themes.Units.iconSizes.medium
        z: 1001
        opacity: mouseArea.containsMouse || hovered ? 1.0 : 0.0
        onClicked: window.clientWindow.close()

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: Themes.Units.shortDuration
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        z: 1000
        onClicked: root.clicked(window)
    }
}
