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
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Ui 1.0 as FluidUi
import "../components" as CustomComponents

Item {
    property int indexOfThisDelegate: index
    property string appId: model.appId
    property bool active: true

    id: root
    width: itemSize
    height: width

    Behavior on width {
        NumberAnimation {
            easing.type: Easing.Linear
            duration: FluidUi.Units.shortDuration
        }
    }

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.Linear
            duration: FluidUi.Units.shortDuration
        }
    }

    FluidUi.Icon {
        id: icon
        anchors.centerIn: parent
        iconName: model.iconName
        width: iconSize
        height: width
        cache: false
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: FluidUi.Units.dp(2)
        color: model.active ? Material.accentColor : "white"
        opacity: model.running ? 1.0 : 0.0

        Behavior on color {
            ColorAnimation {
                easing.type: Easing.OutQuad
                duration: FluidUi.Units.shortDuration
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: FluidUi.Units.shortDuration
            }
        }
    }

    Rectangle {
        id: badge
        anchors {
            top: parent.top
            right: parent.right
            topMargin: -(FluidUi.Units.smallSpacing * 0.35)
            rightMargin: -(FluidUi.Units.largeSpacing * 0.35)
        }
        width: FluidUi.Units.iconSizes.smallMedium
        height: width
        radius: width * 0.5
        color: "orangered"
        opacity: model.hasCount > 0 ? 1.0 : 0.0

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
            text: model.count
        }
    }

    CustomComponents.Tooltip {
        id: tooltip
        text: model.name
        visualLayer: panel.parent
        visualParent: root
    }

    LauncherMenu {
        id: menu
        x: 0
        y: -height
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        onEntered: tooltip.open()
        onExited: tooltip.close()
        onClicked: {
            switch (mouse.button) {
            case Qt.LeftButton:
                if (model.running) {
                    if (model.active)
                        toggleWindows();
                    else
                        activateWindows();
                } else {
                    if (!listView.model.get(index).launch())
                        console.warn("Failed to run:", model.appId);
                }
                break;
            case Qt.RightButton:
                if (menu.showing)
                    menu.close();
                else
                    menu.open();
                break;
            default:
                break;
            }
        }
        onPressAndHold: {
            if (menu.showing)
                menu.close();
            else
                menu.open();
        }
    }

    function activateWindows() {
        // Set index so that the window have a clue of which icon was clicked
        listView.currentIndex = index;

        // Activate all windows of this application and unminimize
        var i, window;
        for (i = 0; i < hawaiiCompositor.windowsModel.count; i++) {
            window = hawaiiCompositor.windowsModel.get(i).window;
            if (window.appId === model.appId) {
                window.minimized = false;
                window.activate();
            }
        }

        // Toggle active flag
        root.active = !root.active;
    }

    function toggleWindows() {
        // Set index so that the window have a clue of which icon was clicked
        listView.currentIndex = index;

        // Minimize or unminimize windows
        var i, window;
        for (i = 0; i < hawaiiCompositor.windowsModel.count; i++) {
            window = hawaiiCompositor.windowsModel.get(i).window;
            if (window.appId === model.appId) {
                var pt = screenView.mapFromItem(root, root.width * 0.5, root.height * 0.5);
                pt.x += output.position.x;
                pt.y += output.position.y;

                window.taskIconGeometry = Qt.rect(pt.x, pt.y, root.width, root.height);
                window.minimized = !window.minimized;
            }
        }
    }

    Component.onDestruction: {
        tooltip.close();
        menu.close();
    }
}
