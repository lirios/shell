/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini
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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Fluid.Controls 1.0 as FluidControls
import Liri.Shell 1.0 as LiriShell

LiriShell.PanelItem {
    id: launcherItem

    property int indexOfThisDelegate: index
    property string appId: model.appId

    highlightColor: model.starting ? Material.color(Material.Orange) : "white"
    highlightOpacity: model.active || model.starting ? 1 : model.running ? 0.4 : 0
    active: model.active

    ToolTip.text: model.name || model.appId
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.visible: containsMouse && !menu.visible

    onClicked: {
        if (model.running) {
            if (model.active)
                toggleWindows();
            else
                compositor.activateShellSurfaces(model.appId);
        } else {
            if (!launcher.model.get(index).launch())
                console.warn("Failed to run:", model.appId);
        }
    }

    onRightClicked: {
        if (menu.visible)
            menu.close();
        else
            menu.open();
    }

    SequentialAnimation on y {
        loops: Animation.Infinite
        running: model.starting
        alwaysRunToEnd: true

        NumberAnimation {
            from: 0; to: -launcherItem.height/2
            easing.type: Easing.OutExpo; duration: 300
        }

        NumberAnimation {
            from: -launcherItem.height/2; to: 0
            easing.type: Easing.OutBounce; duration: 1000
        }

        PauseAnimation { duration: 500 }
    }

    FluidControls.Icon {
        id: icon
        anchors.centerIn: parent
        name: model.iconName || "application-x-executable"
        size: parent.height * 0.55
    }

    Rectangle {
        id: badge
        anchors {
            top: parent.top
            right: parent.right
            topMargin: -(FluidControls.Units.smallSpacing * 0.35)
            rightMargin: -(FluidControls.Units.largeSpacing * 0.35)
        }
        width: FluidControls.Units.iconSizes.smallMedium
        height: width
        radius: width * 0.5
        color: "orangered"
        opacity: model.hasCount > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: FluidControls.Units.shortDuration
            }
        }

        Label {
            anchors.centerIn: parent
            font.pixelSize: parent.width - FluidControls.Units.smallSpacing
            color: "white"
            text: model.count
        }
    }

    LauncherMenu {
        id: menu
        x: 0
        y: -height
    }

    function toggleWindows() {
        // Set index so that the window have a clue of which icon was clicked
        launcher.currentIndex = index;

        // Minimize or unminimize shell surfaces
        for (var i = 0; i < compositor.shellSurfaces.count; i++) {
            var shellSurface = compositor.shellSurfaces.get(i).shellSurface;
            if (shellSurface.canonicalAppId === model.appId) {
                // Task icon position
                var pt = screenView.mapFromItem(launcherItem, launcherItem.width * 0.5, launcherItem.height * 0.5);
                pt.x += output.position.x;
                pt.y += output.position.y;

                // Set task icon geometry and toggle minimization
                for (var j = 0; j < compositor.outputs.length; j++) {
                    var curOutput = compositor.outputs[j];
                    for (var surface in curOutput.viewsBySurface) {
                        var view = curOutput.viewsBySurface[surface];
                        view.taskIconGeometry = Qt.rect(pt.x, pt.y, launcherItem.width, launcherItem.height);
                    }
                }

                // Toggle minimization
                shellSurface.minimized = !shellSurface.minimized;
            }
        }
    }

    Component.onDestruction: {
        menu.close();
    }
}
