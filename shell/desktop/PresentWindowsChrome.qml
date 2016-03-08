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
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import GreenIsland 1.0 as GreenIsland
import Fluid.Ui 1.0 as FluidUi
import "../components" as ShellComponents

Item {
    property var window
    readonly property alias container: mouseArea
    readonly property int margin: FluidUi.Units.dp(10)

    signal activated(var window)
    signal closeRequested(var window)

    id: chrome
    width: window.windowGeometry.width + (2 * margin)
    height: window.windowGeometry.height + (2 * margin)

    XAnimator {
        id: xAnimator
        target: chrome
        from: x
        easing.type: Easing.OutQuad
        duration: FluidUi.Units.mediumDuration
        running: false
    }

    YAnimator {
        id: yAnimator
        target: chrome
        from: y
        easing.type: Easing.OutQuad
        duration: FluidUi.Units.mediumDuration
        running: false
    }

    Behavior on width {
        SmoothedAnimation {
            easing.type: Easing.OutQuad
            duration: FluidUi.Units.mediumDuration
        }
    }

    Behavior on height {
        SmoothedAnimation {
            easing.type: Easing.OutQuad
            duration: FluidUi.Units.mediumDuration
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        z: 4
        onClicked: chrome.activated(window)
    }

    GreenIsland.WaylandQuickItem {
        anchors.fill: parent
        z: 2
        surface: window.surface
        sizeFollowsSurface: false
        inputEventsEnabled: false
        view.discardFrontBuffers: true
    }

    RectangularGlow {
        anchors.fill: parent
        glowRadius: margin
        spread: 0.2
        color: Themes.Theme.palette.view.selectedBackgroundColor
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
        color: "#80000000"
        width: Math.max(parent.width * 0.8, titleLabel.paintedWidth) + FluidUi.Units.smallSpacing * 2
        height: titleLabel.paintedHeight + FluidUi.Units.smallSpacing * 2
        z: 4

        Text {
            id: titleLabel
            anchors {
                centerIn: parent
                margins: FluidUi.Units.smallSpacing
            }
            renderType: Text.NativeRendering
            text: window.title ? window.title : qsTr("Unknown")
            elide: Text.ElideRight
            color: "white"
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
        color: "#80000000"
        width: icon.width + FluidUi.Units.smallSpacing * 2
        height: width
        z: 4

        Components.Icon {
            id: icon
            anchors {
                centerIn: parent
                margins: FluidUi.Units.smallSpacing
            }
            iconName: window.iconName ? window.iconName : "unknown"
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

    Timer {
        id: hideBadgesTimer
        interval: FluidUi.Units.longDuration * 0.5
        onTriggered: {
            // Hide the badges so that chrome will reseamble more the window
            titleBadgeOpacityAnimator.from = 1.0;
            titleBadgeOpacityAnimator.to = 0.0;
            titleBadgeOpacityAnimator.start();
            iconBadgeOpacityAnimator.from = 1.0;
            iconBadgeOpacityAnimator.to = 0.0;
            iconBadgeOpacityAnimator.start();

            // Start autodestruction
            autodestructionTimer.start();
        }
    }

    Timer {
        id: autodestructionTimer
        interval: FluidUi.Units.mediumDuration * 0.5
        onTriggered: {
            // Show original views
            var i;
            for (i = 0; i < chrome.window.views.length; i++)
                chrome.window.views[i].visible = true;

            // Destroy
            chrome.destroy();
        }
    }

    function move(x, y) {
        xAnimator.to = x;
        yAnimator.to = y;
        xAnimator.start();
        yAnimator.start();
    }

    function resize(scale) {
        chrome.width *= scale;
        chrome.height *= scale;
    }

    function triggerAutodestruction() {
        // Autodestruction sequence starts with hiding badges
        hideBadgesTimer.start();
    }
}
