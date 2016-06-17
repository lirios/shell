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

import QtQuick 2.0
import QtQuick.Layouts 1.0
import Qt.labs.controls 1.0
import Qt.labs.controls.material 1.0
import GreenIsland 1.0 as GreenIsland
import Fluid.Ui 1.0 as FluidUi

Popup {
    readonly property real thumbnailSize: FluidUi.Units.dp(200)

    id: windowSwitcher
    focus: true
    modal: true
    implicitWidth: output.availableGeometry.width * 0.7
    implicitHeight: thumbnailSize + label.paintedHeight + (2 * layout.spacing) + FluidUi.Units.largeSpacing

    Component {
        id: thumbnailComponent

        Rectangle {
            readonly property string title: window.title ? window.title : qsTr("Untitled")
            readonly property real ratio: window.surface.size.width / window.surface.size.height

            id: wrapper
            width: height * ratio
            height: thumbnailSize
            color: wrapper.ListView.isCurrentItem ? Material.accent : "transparent"
            radius: FluidUi.Units.dp(4)

            GreenIsland.WaylandQuickItem {
                id: windowItem
                anchors {
                    fill: parent
                    margins: FluidUi.Units.smallSpacing
                }
                surface: window.surface
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

            FluidUi.Icon {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }
                width: FluidUi.Units.iconSizes.large
                height: width
                iconName: window.iconName
                cache: false
                z: 1
            }
        }
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        spacing: FluidUi.Units.smallSpacing

        ListView {
            id: listView
            clip: true
            focus: true
            orientation: ListView.Horizontal
            model: hawaiiCompositor.windowsModel
            spacing: FluidUi.Units.smallSpacing
            highlightMoveDuration: FluidUi.Units.shortDuration
            delegate: thumbnailComponent
            currentIndex: -1

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Label {
            id: label
            text: listView.currentItem ? listView.currentItem.title : ""
            wrapMode: Text.Wrap
            font.bold: true
            maximumLineCount: 2
            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true
        }
    }

    function activate() {
        var window = listView.model.get(listView.currentIndex);
        if (window && window)
            window.window.activate();
    }

    function previous() {
        if (windowsModel.count < 2)
            return;

        if (listView.currentIndex == 0)
            listView.currentIndex = listView.count == 0 ? 0 : listView.count - 1;
        else
            listView.currentIndex--;
        open();
    }

    function next() {
        if (windowsModel.count < 2)
            return;

        if (listView.currentIndex == listView.count - 1)
            listView.currentIndex = 0;
        else
            listView.currentIndex++;
        open();
    }
}
