/****************************************************************************
 * This file is part of Hawaii Shell.
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

import QtQuick 2.1
import QtQuick.Controls 1.0
import Hawaii.Shell.Desktop 0.1

Item {
    id: launcher

    // Icon size
    property int iconSize: {
        switch (settings.iconSize) {
        case LauncherSettings.SmallIconSize:
            return 32;
        case LauncherSettings.MediumIconSize:
            return 48;
        case LauncherSettings.LargeIconSize:
            return 64;
        case LauncherSettings.HugeIconSize:
            return 96;
        }
    }

    // Tile size
    property int tileSize: iconSize + (iconSize / 4)

    // Orientation and alignment
    property alias alignment: settings.alignment
    property alias orientation: view.orientation

    // Number of items
    property alias count: view.count

    // Launcher window
    property var window

    // AppChooser
    Component {
        id: appChooserComponent

        AppChooserWindow {}
    }

    Loader {
        id: appChooserLoader
        sourceComponent: appChooserComponent
        onLoaded: moveAppChooser()
    }

    property alias appChooser: appChooserLoader.item

    // AppChooser window follows the Launcher window when its geometry changes
    Connections {
        id: windowConnection
        onXChanged: moveAppChooser()
        onYChanged: moveAppChooser()
        onWidthChanged: moveAppChooser()
        onHeightChanged: moveAppChooser()
    }

    onWindowChanged: windowConnection.target = window
    /*
    onApplicationDropped: visualModel.model.pinApplication(path)
    onUrlDropped: visualModel.model.pinUrl(url)
    */

    LauncherSettings {
        id: settings
        onAlignmentChanged: moveAppChooser()
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

            onPressed: held = model.draggable
            onPressAndHold: held = model.draggable
            onReleased: held = false
            onClicked: {
                var item = launcherModel.get(index);

                if (mouse.button === Qt.LeftButton)
                    item.activate();
                else if (mouse.button === Qt.MidButton)
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
                        left: parent.left
                        top: parent.top
                        margins: (tileSize - iconSize) / 2
                    }
                    source: "image://desktoptheme/" + (model.iconName ? model.iconName : "unknown")
                    sourceSize: Qt.size(width, height)
                    width: iconSize
                    height: iconSize
                    scale: 1.0

                    Behavior on width {
                        NumberAnimation { easing.type: Easing.Linear; duration: 550 }
                    }
                    Behavior on height {
                        NumberAnimation { easing.type: Easing.Linear; duration: 550 }
                    }
                }

                /*
                FluidUi.BusyIndicator {
                    id: busyIndicator
                    anchors {
                        fill: parent
                        margins: tileSize / 4
                    }
                    running: item.running && !item.active
                    visible: item.running && !item.active
                }
                */

                Image {
                    id: runningIndicator

                    mirror: Qt.application.layoutDirection == Qt.RightToLeft
                    source: "qrc:///images/launcher_dot.png"
                    visible: model.running && !dragArea.held

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
                    visible: model.counterVisible && model.counter >= 0

                    Label {
                        id: counterText

                        anchors.centerIn: parent

                        font.pixelSize: parent.height - 3
                        width: parent.width - 5
                        smooth: true
                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignHCenter
                        // TODO: Color from theme
                        color: "white"
                        text: model.counter
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
        model: LauncherModel {
            id: launcherModel
        }
        delegate: dragDelegate
    }

    ListView {
        id: view
        anchors.fill: parent
        focus: true
        model: visualModel
        cacheBuffer: 10000
        interactive: false
        header: AppChooserButton {
            Connections {
                target: appChooser
                onVisibleChanged: checked = appChooser.visible
            }

            width: tileSize
            height: tileSize
            onCheckedChanged: appChooser.visible = checked
        }
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 250 }
            NumberAnimation { property: "scale"; from: 0.0; to: 1.0; duration: 150 }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 250; easing.type: Easing.OutBounce }
        }
    }

    function moveAppChooser() {
        if (typeof(window) == "undefined")
            return;

        switch (alignment) {
        case LauncherSettings.LeftAlignment:
            appChooser.x = window.x + window.width;
            appChooser.y = window.y;
            break;
        case LauncherSettings.RightAlignment:
            appChooser.x = window.x - appChooser.width;
            appChooser.y = window.y;
            break;
        case LauncherSettings.BottomAlignment:
            appChooser.x = window.x;
            appChooser.y = window.y - appChooser.height;
            break;
        }
    }
}
