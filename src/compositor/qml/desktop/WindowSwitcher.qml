/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtWayland.Compositor 1.0
import Fluid.Core 1.0 as FluidCore
import Fluid.Controls 1.0 as FluidControls

Popup {
    readonly property real thumbnailSize: 200

    id: windowSwitcher
    focus: true
    modal: true
    implicitWidth: output.availableGeometry.width * 0.7
    implicitHeight: thumbnailSize + label.paintedHeight + (2 * layout.spacing) + FluidControls.Units.largeSpacing

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    Component {
        id: thumbnailComponent

        Rectangle {
            readonly property var chrome: liriCompositor.defaultOutput.viewsBySurface[model.window.surface]
            readonly property string title: model.window.title ? model.window.title : qsTr("Untitled")
            readonly property real ratio: chrome ? chrome.width / chrome.height : 0

            id: wrapper
            width: height * ratio
            height: thumbnailSize
            color: wrapper.ListView.isCurrentItem ? Material.accent : "transparent"
            radius: 4

            ShaderEffectSource {
                anchors {
                    fill: parent
                    margins: FluidControls.Units.smallSpacing
                }
                sourceItem: chrome
                z: 0

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.AllButtons
                    onClicked: listView.currentIndex = index
                }
            }

            FluidControls.Icon {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }
                width: FluidControls.Units.iconSizes.large
                height: width
                name: model.window.iconName || "unknown"
                cache: false
                z: 1
            }
        }
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        spacing: FluidControls.Units.smallSpacing

        ListView {
            id: listView
            clip: true
            focus: true
            orientation: ListView.Horizontal
            model: FluidCore.SortFilterProxyModel {
                sourceModel: liriCompositor.shellSurfaces
                filterExpression: model.window.parentSurface === null
            }
            spacing: FluidControls.Units.smallSpacing
            highlightMoveDuration: FluidControls.Units.shortDuration
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
        if (listView.currentItem)
            listView.currentItem.chrome.takeFocus();
    }

    function previous() {
        if (listView.count < 2)
            return;

        if (listView.currentIndex == listView.count)
            listView.currentIndex -= 2;
        else if (listView.currentIndex == 0)
            listView.currentIndex = listView.count == 0 ? 0 : listView.count - 1;
        else
            listView.currentIndex--;
        open();
    }

    function next() {
        if (listView.count < 2)
            return;

        if (listView.currentIndex >= listView.count - 1)
            listView.currentIndex = 0;
        else
            listView.currentIndex++;
        open();
    }
}
