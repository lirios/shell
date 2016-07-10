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

import QtQuick 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Hawaii.Themes 1.0 as Themes
import GreenIsland 1.0 as GreenIsland
import Fluid.UI 1.0 as FluidUi
import "../components" as ShellComponents

Item {
    property var window
    readonly property alias container: mouseArea
    readonly property int margin: FluidUi.Units.dp(10)

    signal activated(var window)
    signal closeRequested(var window)

    id: chrome
    anchors.fill: parent

    Material.theme: Material.Dark

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        z: 4
        onClicked: chrome.activated(window)
    }

    RectangularGlow {
        anchors.fill: parent
        glowRadius: margin
        spread: 0.2
        color: Material.accent
        cornerRadius: FluidUi.Units.gu(0.2)
        opacity: closeButton.opacity > 0.0 ? 0.5 : 0.0
        z: 3

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: FluidUi.Units.shortDuration
            }
        }
    }

    Rectangle {
        id: titleBadge
        anchors.centerIn: parent
        radius: FluidUi.Units.dp(6)
        color: Material.dialogColor
        width: Math.max(parent.width * 0.8, titleLabel.paintedWidth) + FluidUi.Units.smallSpacing * 2
        height: titleLabel.paintedHeight + FluidUi.Units.smallSpacing * 2
        z: 4

        Label {
            id: titleLabel
            anchors {
                centerIn: parent
                margins: FluidUi.Units.smallSpacing
            }
            text: window.title ? window.title : qsTr("Unknown")
            elide: Text.ElideRight
        }

        OpacityAnimator {
            id: titleBadgeOpacityAnimator
            target: titleBadge
            from: 0.0
            to: 1.0
            duration: FluidUi.Units.longDuration
            running: true
        }
    }

    Rectangle {
        id: iconBadge
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: FluidUi.Units.largeSpacing
            bottomMargin: FluidUi.Units.largeSpacing
        }
        radius: FluidUi.Units.dp(6)
        color: Material.dialogColor
        width: icon.width + FluidUi.Units.smallSpacing * 2
        height: width
        z: 4

        FluidUi.Icon {
            id: icon
            anchors {
                centerIn: parent
                margins: FluidUi.Units.smallSpacing
            }
            name: window.iconName ? window.iconName : "unknown"
            width: FluidUi.Units.iconSizes.large
            height: width
        }

        OpacityAnimator {
            id: iconBadgeOpacityAnimator
            target: iconBadge
            from: 0.0
            to: 1.0
            duration: FluidUi.Units.longDuration
            running: true
        }
    }

    ShellComponents.CloseButton {
        id: closeButton
        anchors {
            top: mouseArea.top
            right: mouseArea.right
            margins: -FluidUi.Units.gu(1)
        }
        width: FluidUi.Units.iconSizes.medium
        z: 5
        opacity: mouseArea.containsMouse || hovered ? 1.0 : 0.0
        onClicked: chrome.closeRequested(window)

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: FluidUi.Units.shortDuration
            }
        }
    }
}
