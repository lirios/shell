/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaii.launcher 0.1 as Launcher
import "components" as CustomComponents

Item {
    property real iconSize
    property int alignment
    readonly property real itemSize: iconSize + (iconSize * 0.25)
    readonly property real itemPadding: Themes.Units.smallSpacing * 2
    property alias orientation: listView.orientation
    readonly property alias currentItem: listView.currentItem

    id: launcher

    Component {
        id: iconDelegate

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
                visualParent: root
            }

            CustomComponents.PopupMenu {
                id: menu
                content: [
                    Repeater {
                        model: listView.model.get(root.indexOfThisDelegate).windows

                        CustomComponents.MenuItem {
                            text: modelData.title
                        }
                    },
                    CustomComponents.MenuSeparator {
                        visible: model.hasWindows
                    },
                    /*
                    Repeater {
                        model: menu.actionList ? menu.actionList : 0

                        CustomComponents.MenuItem {
                            text: "Action " + index
                        }
                    },
                    */
                    CustomComponents.MenuSeparator {
                        visible: model.hasActionList
                    },
                    CustomComponents.MenuItem {
                        text: qsTr("New Window")
                        visible: model.running
                    },
                    CustomComponents.MenuSeparator {},
                    CustomComponents.MenuItem {
                        text: qsTr("Add To Launcher")
                        visible: !model.pinned
                        onClicked: model.pinned = true
                    },
                    CustomComponents.MenuItem {
                        text: qsTr("Remove From Launcher")
                        visible: model.pinned
                        onClicked: model.pinned = false
                    },
                    CustomComponents.MenuSeparator {},
                    CustomComponents.MenuItem {
                        text: qsTr("Show All Windows")
                        visible: model.running
                    },
                    CustomComponents.MenuItem {
                        text: qsTr("Show")
                        visible: model.running && !model.active
                    },
                    CustomComponents.MenuItem {
                        text: qsTr("Hide")
                        visible: model.running && model.active
                    },
                    CustomComponents.MenuSeparator {},
                    CustomComponents.MenuItem {
                        text: qsTr("Quit")
                        visible: model.running
                    }
                ]
                onTriggered: {
                    mouseArea.hoverEnabled = false;
                    tooltip.close();
                }
                onClosed: mouseArea.hoverEnabled = true
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                hoverEnabled: true
                onPositionChanged: if (hoverEnabled) tooltip.open()
                onExited: tooltip.close()
                onClicked: {
                    switch (mouse.button) {
                    case Qt.LeftButton:
                        toggleWindows();
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
            }

            function toggleWindows() {
                // Set index so that the window have a clue of which icon was clicked
                listView.currentIndex = index;

                // Minimize or unminimize windows
                var i, entry;
                for (i = 0; i < surfaceModel.count; i++) {
                    entry = surfaceModel.get(i);
                    if (entry.window.appId === model.appId) {
                        if (root.active)
                            entry.window.minimize();
                        else
                            entry.window.unminimize();
                    }
                }

                // Toggle active flag
                root.active = !root.active;
            }
        }
    }

    ListView {
        id: listView
        anchors.centerIn: parent
        orientation: alignment == Qt.AlignTop || alignment == Qt.AlignBottom ? ListView.Horizontal : ListView.Vertical
        spacing: Themes.Units.smallSpacing
        interactive: false
        add: Transition {
            NumberAnimation { properties: "scale"; from: 0.1; to: 1.0; duration: Themes.Units.shortDuration }
        }
        populate: Transition {
            NumberAnimation { properties: "scale"; from: 0.1; to: 1.0; duration: Themes.Units.longDuration }
        }
        model: Launcher.LauncherModel {
            id: launcherModel
        }
        delegate: iconDelegate
        width: calcWidth()
        height: calcHeight()

        function calcWidth() {
            if (orientation == ListView.Horizontal)
                return contentWidth;
            return itemSize + (itemPadding / 2);
        }

        function calcHeight() {
            if (orientation == ListView.Vertical)
                return contentHeight;
            return itemSize + (itemPadding / 2);
        }
    }
}
