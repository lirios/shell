/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import GreenIsland 1.0 as GreenIsland
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

Rectangle {
    signal closed()

    id: root
    color: "#80000000"
    radius: Themes.Units.gu(0.5)
    opacity: 0.0
    //width: Math.min(listView.count * (listView.thumbnailWidth + Themes.Units.largeSpacing * 2), screenView.width * 0.7)
    //height: Math.min(listView.thumbnailHeight + label.paintedHeight * 2 + Themes.Units.largeSpacing * 2, screenView.height * 0.5)
    width: screenView.width * 0.7
    height: screenView.height * 0.5

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InSine
            duration: Themes.Units.shortDuration
        }
    }

    // Keyboard event handling
    GreenIsland.KeyEventFilter {
        Keys.onReleased: {
            if (event.modifiers & Qt.MetaModifier) {
                // Cycle between windows
                if (event.key === Qt.Key_Tab) {
                    // Next
                    if (listView.currentIndex == listView.count - 1)
                        listView.currentIndex = 0;
                    else
                        listView.currentIndex++;
                } else if (event.key === Qt.Key_Backtab) {
                    // Previous
                    if (listView.currentIndex == 0)
                        listView.currentIndex = listView.count - 1;
                    else
                        listView.currentIndex--;
                }
            } else {
                // Give focus to the selected window
                var item = listModel.get(listView.currentIndex);
                if (item && item.surface)
                    item.surface.activate();

                // Keys released, deactivate switcher
                root.closed();
            }

            event.accepted = true;
        }
    }

    Component {
        id: thumbnailComponent

        Rectangle {
            readonly property string title: modelData.child.surface.title
            readonly property alias surface: windowItem.surface

            id: wrapper
            width: listView.thumbnailWidth
            height: listView.thumbnailHeight
            color: wrapper.ListView.isCurrentItem ? Themes.Theme.palette.panel.selectedBackgroundColor : "transparent"
            radius: Themes.Units.gu(0.5)

            GreenIsland.WaylandQuickItem {
                id: windowItem
                anchors {
                    fill: parent
                    margins: Themes.Units.smallSpacing
                }
                surface: thumbnailSurface
                sizeFollowsSurface: false
                inputEventsEnabled: false
                view.discardFrontBuffers: true
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
                //iconName: modelData.clientWindow.iconName
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
            readonly property real ratio: screenView.width / screenView.height

            id: listView
            clip: true
            orientation: ListView.Horizontal
            model: ListModel {
                id: listModel
            }
            spacing: Themes.Units.smallSpacing
            highlightMoveDuration: Themes.Units.shortDuration
            delegate: thumbnailComponent
            currentIndex: 0

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
        // Populate list model

        // Show with an animtation
        opacity = 1.0;
    }
}
