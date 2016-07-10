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
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.1
import QtQml.Models 2.2
import GreenIsland 1.0
import Fluid.UI 1.0 as FluidUi
import Hawaii.Themes 1.0 as Themes
import "indicators"
import "launcher"

Rectangle {
    readonly property alias launcherIndicator: launcherIndicator
    readonly property alias currentLauncherItem: launcher.currentItem
    property real size: FluidUi.Units.iconSizes.large
    readonly property real spacing: FluidUi.Units.smallSpacing
    readonly property int orientation: {
        if (state == "left" || state == "right")
            return Qt.Vertical;
        return Qt.Horizontal;
    }

    signal indicatorTriggered(var indicator)

    id: panel
    color: "transparent"
    state: "bottom"
    states: [
        State {
            name: "left"

            PropertyChanges {
                target: panel
                width: panel.size + panel.spacing
                height: parent.height
            }
            AnchorChanges {
                target: panel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.right: undefined
                anchors.bottom: parent.bottom
            }

            PropertyChanges {
                target: mainLayout
                rows: 3
                columns: 1
                flow: GridLayout.TopToBottom
                rowSpacing: 0
                columnSpacing: FluidUi.Units.smallSpacing
            }
            PropertyChanges {
                target: launcher
                orientation: ListView.Vertical
            }

            PropertyChanges {
                target: indicatorsView
                rows: 6
                columns: 1
                flow: GridLayout.LeftToRight
                rowSpacing: FluidUi.Units.smallSpacing
                columnSpacing: 0
            }

            StateChangeScript {
                script: setAvailableGeometry()
            }
        },
        State {
            name: "top"

            PropertyChanges {
                target: panel
                width: parent.width
                height: panel.size + panel.spacing
            }
            AnchorChanges {
                target: panel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: undefined
            }

            PropertyChanges {
                target: mainLayout
                rows: 1
                columns: 3
                flow: GridLayout.LeftToRight
                rowSpacing: FluidUi.Units.smallSpacing
                columnSpacing: 0
            }
            PropertyChanges {
                target: launcher
                orientation: ListView.Horizontal
            }

            PropertyChanges {
                target: indicatorsView
                rows: 1
                columns: 6
                flow: GridLayout.LeftToRight
                rowSpacing: 0
                columnSpacing: FluidUi.Units.smallSpacing
            }

            StateChangeScript {
                script: setAvailableGeometry()
            }
        },
        State {
            name: "right"

            PropertyChanges {
                target: panel
                width: panel.size + panel.spacing
                height: parent.height
            }
            AnchorChanges {
                target: panel
                anchors.left: undefined
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

            PropertyChanges {
                target: mainLayout
                rows: 3
                columns: 1
                flow: GridLayout.TopToBottom
                rowSpacing: 0
                columnSpacing: FluidUi.Units.smallSpacing
            }
            PropertyChanges {
                target: launcher
                orientation: ListView.Vertical
            }

            PropertyChanges {
                target: indicatorsView
                rows: 6
                columns: 1
                flow: GridLayout.LeftToRight
                rowSpacing: FluidUi.Units.smallSpacing
                columnSpacing: 0
            }

            StateChangeScript {
                script: setAvailableGeometry()
            }
        },
        State {
            name: "bottom"

            PropertyChanges {
                target: panel
                width: parent.width
                height: panel.size + panel.spacing
            }
            AnchorChanges {
                target: panel
                anchors.left: parent.left
                anchors.top: undefined
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

            PropertyChanges {
                target: mainLayout
                rows: 1
                columns: 3
                flow: GridLayout.LeftToRight
                rowSpacing: FluidUi.Units.smallSpacing
                columnSpacing: 0
            }
            PropertyChanges {
                target: launcher
                orientation: ListView.Horizontal
            }

            PropertyChanges {
                target: indicatorsView
                rows: 1
                columns: 6
                flow: GridLayout.LeftToRight
                rowSpacing: 0
                columnSpacing: FluidUi.Units.smallSpacing
            }

            StateChangeScript {
                script: setAvailableGeometry()
            }
        }
    ]
    transitions: Transition {
        AnchorAnimation {
            easing.type: Easing.OutQuad
            duration: FluidUi.Units.shortDuration
        }
    }
    onWidthChanged: setAvailableGeometry()
    onHeightChanged: setAvailableGeometry()

    Behavior on color {
        ColorAnimation { duration: FluidUi.Units.longDuration }
    }

    Behavior on width {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: FluidUi.Units.shortDuration
        }
    }

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: FluidUi.Units.shortDuration
        }
    }

    /*
     * Focused window
     */

    Connections {
        target: hawaiiCompositor.applicationManager
        onFocusedWindowChanged: setup()
    }

    Connections {
        target: hawaiiCompositor.applicationManager.focusedWindow
        onMaximizedChanged: setup()
    }

    /*
     * Layout
     */

    GridLayout {
        id: mainLayout
        anchors.fill: parent

        Rectangle {
            radius: FluidUi.Units.dp(6)
            color: Material.dialogColor
            implicitWidth: launcherIndicator.width + FluidUi.Units.smallSpacing
            implicitHeight: launcherIndicator.height + FluidUi.Units.smallSpacing

            LauncherIndicator {
                id: launcherIndicator
            }

            Layout.alignment: Qt.AlignCenter
        }

        Launcher {
            id: launcher
            iconSize: panel.size
            itemSize: panel.size + panel.spacing

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            radius: FluidUi.Units.dp(6)
            color: Material.dialogColor
            implicitWidth: indicatorsView.implicitWidth + FluidUi.Units.smallSpacing
            implicitHeight: indicatorsView.implicitHeight + FluidUi.Units.smallSpacing

            GridLayout {
                property real iconSize: {
                    switch (panel.size) {
                    case FluidUi.Units.iconSizes.medium:
                        return FluidUi.Units.iconSizes.small;
                    case FluidUi.Units.iconSizes.large:
                        return FluidUi.Units.iconSizes.smallMedium;
                    case FluidUi.Units.iconSizes.huge:
                        return FluidUi.Units.iconSizes.medium;
                    default:
                        break;
                    }

                    return FluidUi.Units.iconSizes.smallMedium;
                }

                id: indicatorsView

                EventsIndicator {
                    iconSize: indicatorsView.iconSize
                    onTriggered: indicatorTriggered(caller)
                }

                SettingsIndicator {
                    iconSize: indicatorsView.iconSize
                    onTriggered: indicatorTriggered(caller)
                }

                SoundIndicator {
                    iconSize: indicatorsView.iconSize
                    onTriggered: indicatorTriggered(caller)
                }

                NetworkIndicator {
                    iconSize: indicatorsView.iconSize
                    onTriggered: indicatorTriggered(caller)
                }

                StorageIndicator {
                    iconSize: indicatorsView.iconSize
                    onTriggered: indicatorTriggered(caller)
                }

                BatteryIndicator {
                    iconSize: indicatorsView.iconSize
                    onTriggered: indicatorTriggered(caller)
                }

                Layout.alignment: Qt.AlignCenter
            }
        }
    }

    Component.onCompleted: setAvailableGeometry()

    function setup() {
        // TODO: Don't resize the panel, the window is maximized before we change the available
        // geometry resulting in a "hole" between the window and the panel
        var window = hawaiiCompositor.applicationManager.focusedWindow;
        if (window && window.maximized) {
            color = Themes.Theme.palette.rgba(Material.dialogColor, 0.9);
            //launcher.iconSize = FluidUi.Units.iconSizes.medium;
        } else {
            color = "transparent";
            //launcher.iconSize = FluidUi.Units.iconSizes.large;
        }
    }

    function setAvailableGeometry() {
        switch (state) {
        case "left":
            output.availableGeometry = Qt.rect(width, 0, output.geometry.width - width, output.geometry.height);
            break;
        case "top":
            output.availableGeometry = Qt.rect(0, height, output.geometry.width, output.geometry.height - height);
            break;
        case "right":
            output.availableGeometry = Qt.rect(0, 0, output.geometry.width - width, output.geometry.height);
            break;
        case "bottom":
            output.availableGeometry = Qt.rect(0, 0, output.geometry.width, output.geometry.height - height);
            break;
        }
    }
}
