// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls
import "../components" as ShellComponents

Item {
    property Item view: null
    readonly property int margin: 10

    signal selected(var view)
    signal closed(var view)

    id: chrome

    x: view.x + view.window.windowGeometry.x * view.scale
    y: view.y + view.window.windowGeometry.y * view.scale
    width: view.window.windowGeometry.width * view.scale
    height: view.window.windowGeometry.height * view.scale

    Material.theme: Material.Dark

    HoverHandler {
        id: hoverHandler
    }

    TapHandler {
        onTapped: {
            chrome.selected(view);
        }
    }

    RectangularGlow {
        anchors.fill: parent
        glowRadius: margin
        spread: 0.2
        color: Material.accent
        cornerRadius: FluidControls.Units.gu(0.2)
        opacity: hoverHandler.hovered ? 0.5 : 0.0
        z: 3

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: FluidControls.Units.shortDuration
            }
        }
    }

    Rectangle {
        id: titleBadge
        anchors.centerIn: parent
        radius: 6
        color: Material.dialogColor
        width: Math.max(parent.width * 0.8, titleLabel.paintedWidth) + FluidControls.Units.smallSpacing * 2
        height: titleLabel.paintedHeight + FluidControls.Units.smallSpacing * 2
        z: 4

        Label {
            id: titleLabel
            anchors {
                centerIn: parent
                margins: FluidControls.Units.smallSpacing
            }
            text: view.window.title ? view.window.title : qsTr("Unknown")
            elide: Text.ElideRight
        }

        OpacityAnimator {
            id: titleBadgeOpacityAnimator
            target: titleBadge
            from: 0.0
            to: 1.0
            duration: FluidControls.Units.longDuration
            running: true
        }
    }

    Rectangle {
        id: iconBadge
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: FluidControls.Units.largeSpacing
            bottomMargin: FluidControls.Units.largeSpacing
        }
        radius: 6
        color: Material.dialogColor
        width: icon.width + FluidControls.Units.smallSpacing * 2
        height: width
        z: 4

        FluidControls.Icon {
            id: icon
            anchors {
                centerIn: parent
                margins: FluidControls.Units.smallSpacing
            }
            name: view.window.iconName
            width: FluidControls.Units.iconSizes.large
            height: width
        }

        OpacityAnimator {
            id: iconBadgeOpacityAnimator
            target: iconBadge
            from: 0.0
            to: 1.0
            duration: FluidControls.Units.longDuration
            running: true
        }
    }

    ShellComponents.CloseButton {
        id: closeButton
        anchors {
            top: parent.top
            right: parent.right
            margins: -FluidControls.Units.gu(1)
        }
        width: FluidControls.Units.iconSizes.medium
        z: 5
        opacity: hoverHandler.hovered ? 1.0 : 0.0
        onClicked: chrome.closed(view)

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InCubic
                duration: FluidControls.Units.shortDuration
            }
        }
    }
}
