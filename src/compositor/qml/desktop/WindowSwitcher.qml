// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtWayland.Compositor 1.15
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

                TapHandler {
                    onTapped: {
                        listView.currentIndex = index;
                    }
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
                sourceModel: liriCompositor.windows
                filterExpression: modelData.parentSurface === null
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
