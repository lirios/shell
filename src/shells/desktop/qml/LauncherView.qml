/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import GreenIsland 1.0
import FluidCore 1.0
import FluidUi 1.0

LauncherDropItem {
    id: launcher

    // Icon size
    property int iconSize: settings.value("icon-size")

    // Tile size
    property int tileSize: iconSize + (iconSize / 4)

    // Orientation
    property alias orientation: view.orientation

    // Number of items
    property alias count: view.count

    onApplicationDropped: visualModel.model.pinApplication(path)
    onUrlDropped: visualModel.model.pinUrl(url)

    Settings {
        id: settings
        schema: "org.hawaii.greenisland"
        group: "launcher"
        onValueChanged: {
            var val = settings.value("icon-size");
            if (val)
                iconSize = val;
            else
                iconSize = theme.hugeIconSize;
            tileSize = iconSize + (iconSize / 4);
        }
    }

    LauncherModel {
        id: launcherModel
        objectName: "launcherModel"
    }

    // Launcher view delegate
    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            // Is the user dragging it?
            property bool held: false

            width: tileSize
            height: tileSize

            drag.target: held ? tile : undefined
            drag.axis: Drag.XandYAxis

            onPressed: held = item.draggable
            onPressAndHold: held = item.draggable
            onReleased: held = false
            onClicked: {
                if (mouse.button == Qt.LeftButton) {
                    item.activate();
               } else if (mouse.button == Qt.MidButton)
                   item.launchNewInstance();
            }

            states: [
                State {
                    name: "horizontal"
                    when: orientation == ListView.Horizontal

                    AnchorChanges {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                    }
                },
                State {
                    name: "vertical"
                    when: orientation == ListView.Vertical

                    AnchorChanges {
                        anchors.left: parent.left
                        anchors.right: parent.right
                    }
                }
            ]

            Item {
                id: tile

                anchors.fill: dragArea
                opacity: dragArea.held ? 0.5 : 1.0

                Image {
                    id: icon

                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    anchors.margins: (tileSize - iconSize) / 2
                    smooth: true
                    source: "image://desktoptheme/" + (item.iconName ? item.iconName : "unknown")
                    sourceSize: Qt.size(iconSize, iconSize)
                    scale: 1.0

                    Behavior on width {
                        NumberAnimation { easing.type: Easing.Linear; duration: 550 }
                    }
                    Behavior on height {
                        NumberAnimation { easing.type: Easing.Linear; duration: 550 }
                    }
                }

                BusyIndicator {
                    id: busyIndicator
                    anchors {
                        fill: parent
                        margins: tileSize / 4
                    }
                    running: item.running && !item.active
                    visible: item.running && !item.active
                }

                Image {
                    id: runningIndicator

                    mirror: Qt.application.layoutDirection == Qt.RightToLeft
                    source: "qrc:///images/launcher_dot.png"
                    visible: item.running && !dragArea.held

                    states: [
                        State {
                            name: "horizontal"
                            when: orientation == ListView.Horizontal

                            AnchorChanges {
                                target: runningIndicator
                                anchors.top: icon.bottom
                                anchors.horizontalCenter: icon.horizontalCenter
                            }
                        },
                        State {
                            name: "vertical-ltr"
                            when: orientation == ListView.Vertical && Qt.application.layoutDirection == Qt.LeftToRight

                            AnchorChanges {
                                target: runningIndicator
                                anchors.right: icon.left
                                anchors.verticalCenter: icon.verticalCenter
                            }
                        },
                        State {
                            name: "vertical-rtl"
                            when: orientation == ListView.Vertical && Qt.application.layoutDirection == Qt.RightToLeft

                            AnchorChanges {
                                target: runningIndicator
                                anchors.left: icon.right
                                anchors.verticalCenter: icon.verticalCenter
                            }
                        }
                    ]
                }

                Rectangle {
                    id: counter

                    // TODO: Depends on icon size which will need to be set from settings
                    // anyway at the moment we set it on Component.onCompleted
                    width: 32
                    height: 16

                    anchors.right: {
                        if (Qt.application.layoutDirection == Qt.LeftToRight)
                            return parent.right;
                    }
                    anchors.left: {
                        if (Qt.application.layoutDirection == Qt.RightToLeft)
                            return parent.left;
                    }
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 4
                    smooth: true
                    radius: height / 2 - 1
                    // TODO: Colors from theme
                    border { width: 2; color: "white" }
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#ffabab" }
                        GradientStop { position: 1.0; color: "#ff0000" }
                    }
                    visible: item.counterVisible && item.counter >= 0

                    Text {
                        id: counterText

                        anchors.centerIn: parent

                        font.pixelSize: parent.height - 3
                        width: parent.width - 5
                        smooth: true
                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignHCenter
                        // TODO: Color from theme
                        color: "white"
                        text: item.counter
                    }

                    Component.onCompleted: {
                        // Make it a circle where the text fits in
                        width = counterText.paintedWidth + 6;
                        height = counterText.paintedHeight;
                        radius = width;
                    }
                }

                // TODO: Run an animation that makes the icon "jump" when applications demand attention

                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2

                states: State {
                    when: dragArea.held

                    ParentChange {
                        target: tile
                        parent: launcher
                    }
                    AnchorChanges {
                        target: tile
                        anchors {
                            horizontalCenter: undefined
                            verticalCenter: undefined
                        }
                    }
                }
            }

            DropArea {
                anchors {
                    fill: parent
                    margins: 10
                }

                onEntered: {
                    // FIXME: Look if the item has draggable = true instead
/*
                    if (dragArea.VisualDataModel.itemsIndex == 0)
                        return;
*/

                    visualModel.items.move(
                        drag.source.VisualDataModel.itemsIndex,
                        dragArea.VisualDataModel.itemsIndex);
                }
            }
        }
    }

    VisualDataModel {
        id: visualModel

        model: ListAggregatorModel { id: items }
        delegate: dragDelegate
    }

    Component {
        id: appChooserIcon

        Button {
            checkable: true
            iconSource: "view-grid-symbolic"
            width: tileSize
            height: width
            onClicked: root.appChooser.visible = !root.appChooser.visible
        }
    }

    ListView {
        id: view
        anchors.fill: parent

        focus: true
        model: visualModel
        cacheBuffer: 10000
        interactive: false
        header: appChooserIcon

        add: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 400 }
            NumberAnimation { property: "scale"; from: 0.0; to: 1.0; duration: 400 }
        }

        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
        }
    }

    Component.onCompleted: {
        items.appendModel(launcherModel);
    }
}
