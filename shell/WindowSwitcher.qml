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
import Qt.labs.controls 1.0 as LabsControls
import GreenIsland 1.0 as GreenIsland
import Hawaii.Themes 1.0 as Themes
import Fluid.Ui 1.0 as FluidUi

Rectangle {
    readonly property real thumbnailHeight: FluidUi.Units.dp(250)

    signal closed()

    id: root
    color: "#80000000"
    radius: FluidUi.Units.gu(0.5)
    opacity: 0.0
    width: output.availableGeometry.width * 0.7
    height: Math.min(thumbnailHeight + layout.anchors.margins * 2 + layout.spacing * 2 + label.paintedHeight, output.availableGeometry.height * 0.7)

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InSine
            duration: FluidUi.Units.shortDuration
        }
    }

    // Keyboard event handling
    GreenIsland.KeyEventFilter {
        Keys.onReleased: {
            if (event.key == Qt.Key_Super_L || event.key == Qt.Key_Super_R) {
                // Give focus to the selected window
                var window = listView.model.get(listView.currentIndex);
                if (window)
                    window.active = true;

                // Keys released, deactivate switcher
                root.closed();
            }
        }
    }

    Component {
        id: thumbnailComponent

        Rectangle {
            readonly property string title: window.title
            readonly property real ratio: window.surface.size.width / window.surface.size.height

            id: wrapper
            width: thumbnailHeight * ratio
            height: thumbnailHeight
            color: wrapper.ListView.isCurrentItem ? Themes.Theme.palette.panel.selectedBackgroundColor : "transparent"
            radius: FluidUi.Units.gu(0.5)

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
        anchors {
            fill: parent
            margins: FluidUi.Units.largeSpacing
        }
        spacing: FluidUi.Units.smallSpacing

        ListView {
            id: listView
            clip: true
            orientation: ListView.Horizontal
            model: hawaiiCompositor.windowsModel
            spacing: FluidUi.Units.smallSpacing
            highlightMoveDuration: FluidUi.Units.shortDuration
            delegate: thumbnailComponent
            currentIndex: 0

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        LabsControls.Label {
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

    function previous() {
        if (listView.currentIndex == 0)
            listView.currentIndex = listView.count - 1;
        else
            listView.currentIndex--;
    }

    function next() {
        if (listView.currentIndex == listView.count - 1)
            listView.currentIndex = 0;
        else
            listView.currentIndex++;
    }
}
