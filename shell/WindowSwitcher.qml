/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import GreenIsland 1.0
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

Rectangle {
    id: root
    color: "#80000000"
    radius: Themes.Units.gu(0.5)
    opacity: 0.0
    width: Math.min(listView.count * (listView.thumbnailWidth + Themes.Units.largeSpacing * 2), compositorRoot.width * 0.7)
    height: Math.min(listView.thumbnailHeight + label.paintedHeight * 2 + Themes.Units.largeSpacing * 2, compositorRoot.height * 0.5)

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InSine
            duration: Themes.Units.mediumDuration
        }
    }

    // Keyboard event handling
    Connections {
        target: compositorRoot
        onWindowSwitchPrev: {
            if (listView.currentIndex == 0)
                listView.currentIndex = listView.count - 1;
            else
                listView.currentIndex--;
        }
        onWindowSwitchNext: {
            if (listView.currentIndex == listView.count - 1)
                listView.currentIndex = 0;
            else
                listView.currentIndex++;
        }
        onWindowSwitchSelect: {
            // Give focus to the selected window
            var item = compositorRoot.windowList[listView.currentIndex];
            if (item && item.clientWindow)
                item.clientWindow.activate();
        }
    }

    Component {
        id: thumbnailComponent

        Rectangle {
            readonly property string title: modelData.child.surface.title

            id: wrapper
            width: listView.thumbnailWidth
            height: listView.thumbnailHeight
            color: wrapper.ListView.isCurrentItem ? Themes.Theme.palette.panel.selectedBackgroundColor : "transparent"
            radius: Themes.Units.gu(0.5)

            SurfaceRenderer {
                anchors {
                    fill: parent
                    margins: Themes.Units.smallSpacing
                }
                source: modelData.child
                z: 0

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.AllButtons
                    onClicked: listView.currentIndex = index
                }
            }

            Components.Icon {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }
                width: Themes.Units.iconSizes.large
                height: width
                iconName: modelData.clientWindow.iconName
                cache: false
                z: 1
            }
        }
    }

    ColumnLayout {
        anchors {
            fill: parent
            margins: Themes.Units.largeSpacing
        }
        spacing: Themes.Units.smallSpacing

        ListView {
            readonly property real thumbnailWidth: Themes.Units.gu(24)
            readonly property real thumbnailHeight: thumbnailWidth / ratio
            readonly property real ratio: compositorRoot.width / compositorRoot.height

            id: listView
            clip: true
            orientation: ListView.Horizontal
            model: compositorRoot.windowList
            spacing: Themes.Units.smallSpacing
            highlightMoveDuration: Themes.Units.shortDuration
            delegate: thumbnailComponent
            currentIndex: compositorRoot.activeWindowIndex

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Label {
            id: label
            text: listView.currentItem ? listView.currentItem.title : qsTr("Untitled")
            wrapMode: Text.Wrap
            color: Themes.Theme.palette.panel.textColor
            font.bold: true
            style: Text.Raised
            styleColor: Themes.Theme.palette.panel.textEffectColor
            maximumLineCount: 2
            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true
        }
    }

    Component.onCompleted: {
        // Show with an animtation
        opacity = 1.0;
    }
}
