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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
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
            duration: Themes.Units.shortDuration
        }
    }

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.Linear
            duration: Themes.Units.shortDuration
        }
    }

    Components.Icon {
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
        height: Themes.Units.dp(2)
        color: model.active ? Themes.Theme.palette.view.selectedBackgroundColor : "white"
        opacity: model.running ? 1.0 : 0.0

        Behavior on color {
            ColorAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.shortDuration
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.shortDuration
            }
        }
    }

    Rectangle {
        id: badge
        anchors {
            top: parent.top
            right: parent.right
            topMargin: -(Themes.Units.smallSpacing * 0.35)
            rightMargin: -(Themes.Units.largeSpacing * 0.35)
        }
        width: Themes.Units.iconSizes.smallMedium
        height: width
        radius: width * 0.5
        color: "orangered"
        opacity: model.hasCount > 0 ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.shortDuration
            }
        }

        Label {
            anchors.centerIn: parent
            font.pixelSize: parent.width - Themes.Units.smallSpacing
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
                        activeWindows();
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

    function activeWindows() {
        // Set index so that the window have a clue of which icon was clicked
        listView.currentIndex = index;

        // Minimize or unminimize windows
        var i, entry;
        for (i = 0; i < surfaceModel.count; i++) {
            entry = surfaceModel.get(i);
            if (entry.window.appId === model.appId) {
                if (entry.window.minimized)
                    entry.window.unminimize();
                if (!entry.window.active)
                    entry.window.activate();
            }
        }

        // Toggle active flag
        root.active = !root.active;
    }

    function toggleWindows() {
        // Set index so that the window have a clue of which icon was clicked
        listView.currentIndex = index;

        // Minimize or unminimize windows
        var i, entry;
        for (i = 0; i < surfaceModel.count; i++) {
            entry = surfaceModel.get(i);
            if (entry.window.appId === model.appId) {
                if (entry.window.minimized)
                    entry.window.unminimize();
                else
                    entry.window.minimize();
            }
        }
    }

    Component.onDestruction: {
        tooltip.close();
        menu.close();
    }
}
