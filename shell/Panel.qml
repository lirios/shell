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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import QtQml.Models 2.2
import GreenIsland 1.0
import Hawaii.Themes 1.0 as Themes
import "indicators"
import "launcher"

Rectangle {
    property Indicator selectedIndicator: null
    property Indicator lastIndicator: null
    readonly property alias launcherIndicator: launcherIndicator
    readonly property alias currentLauncherItem: launcher.currentItem
    property real size: Themes.Units.iconSizes.large
    readonly property real spacing: Themes.Units.smallSpacing

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
                columnSpacing: Themes.Units.smallSpacing
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
                rowSpacing: Themes.Units.smallSpacing
                columnSpacing: 0
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
                rowSpacing: Themes.Units.smallSpacing
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
                columnSpacing: Themes.Units.smallSpacing
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
                columnSpacing: Themes.Units.smallSpacing
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
                rowSpacing: Themes.Units.smallSpacing
                columnSpacing: 0
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
                rowSpacing: Themes.Units.smallSpacing
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
                columnSpacing: Themes.Units.smallSpacing
            }
        }
    ]
    transitions: Transition {
        AnchorAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.shortDuration
        }
    }
    onHeightChanged: screenView.setAvailableGeometry(height)

    Behavior on color {
        ColorAnimation { duration: Themes.Units.longDuration }
    }

    Behavior on width {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.shortDuration
        }
    }

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.shortDuration
        }
    }

    Connections {
        target: output
        onActiveWindowChanged: setup()
    }

    Connections {
        target: output.activeWindow
        onMaximizedChanged: setup()
    }

    GridLayout {
        id: mainLayout
        anchors.fill: parent

        LauncherIndicator {
            id: launcherIndicator

            Layout.alignment: Qt.AlignCenter
        }

        Launcher {
            id: launcher
            iconSize: panel.size
            itemSize: panel.size + panel.spacing

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        GridLayout {
            property real iconSize: {
                switch (panel.size) {
                case Themes.Units.iconSizes.medium:
                    return Themes.Units.iconSizes.small;
                case Themes.Units.iconSizes.large:
                    return Themes.Units.iconSizes.smallMedium;
                case Themes.Units.iconSizes.huge:
                    return Themes.Units.iconSizes.medium;
                default:
                    break;
                }

                return Themes.Units.iconSizes.smallMedium;
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
        }
    }

    function setup() {
        // TODO: Don't resize the panel, the window is maximized before we change the available
        // geometry resulting in a "hole" between the window and the panel
        if (output.activeWindow && output.activeWindow.maximized) {
            color = Themes.Theme.palette.rgba(Themes.Theme.palette.window.backgroundColor, 0.85);
            //launcher.iconSize = Themes.Units.iconSizes.medium;
        } else {
            color = "transparent";
            //launcher.iconSize = Themes.Units.iconSizes.large;
        }
    }
}
