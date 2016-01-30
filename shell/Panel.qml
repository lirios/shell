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

    signal indicatorTriggered(var indicator)

    id: root
    color: "transparent"
    state: "hbottom"
    states: [
        State {
            name: "htop"

            PropertyChanges {
                target: root
                width: undefined
                height: launcher.itemSize + launcher.itemPadding
            }
            AnchorChanges {
                target: root
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: undefined
            }

            PropertyChanges {
                target: leftView
                orientation: ListView.Horizontal
                width: leftView.contentWidth
                height: indicators.iconSize + Themes.Units.largeSpacing
            }
            AnchorChanges {
                target: leftView
                anchors.left: root.left
                anchors.verticalCenter: root.verticalCenter
            }

            PropertyChanges {
                target: launcher
                orientation: ListView.Horizontal
                width: root.width - leftView.width - indicatorsView.width
                viewWidth: launcher.contentWidth
                viewHeight: size + (launcher.itemPadding / 2)
            }
            AnchorChanges {
                target: launcher
                anchors.left: leftView.right
                anchors.top: root.top
                anchors.right: indicatorsView.left
                anchors.bottom: root.bottom
            }

            PropertyChanges {
                target: indicatorsView
                orientation: ListView.Horizontal
                width: indicatorsView.contentWidth
                height: indicators.iconSize + Themes.Units.largeSpacing
            }
            AnchorChanges {
                target: indicatorsView
                anchors.right: root.right
                anchors.verticalCenter: root.verticalCenter
            }
        },
        State {
            name: "hbottom"

            PropertyChanges {
                target: root
                width: undefined
                height: launcher.itemSize + launcher.itemPadding
            }
            AnchorChanges {
                target: root
                anchors.left: parent.left
                anchors.top: undefined
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

            PropertyChanges {
                target: leftView
                orientation: ListView.Horizontal
                width: leftView.contentWidth
                height: indicators.iconSize + Themes.Units.largeSpacing
            }
            AnchorChanges {
                target: leftView
                anchors.left: root.left
                anchors.verticalCenter: root.verticalCenter
            }

            PropertyChanges {
                target: launcher
                orientation: ListView.Horizontal
                width: root.width - leftView.width - indicatorsView.width
                viewWidth: launcher.contentWidth
                viewHeight: size + (launcher.itemPadding / 2)
            }
            AnchorChanges {
                target: launcher
                anchors.left: leftView.right
                anchors.top: root.top
                anchors.right: indicatorsView.left
                anchors.bottom: root.bottom
            }

            PropertyChanges {
                target: indicatorsView
                orientation: ListView.Horizontal
                width: indicatorsView.contentWidth
                height: indicators.iconSize + Themes.Units.largeSpacing
            }
            AnchorChanges {
                target: indicatorsView
                anchors.right: root.right
                anchors.verticalCenter: root.verticalCenter
            }
        },
        State {
            name: "vleft"

            PropertyChanges {
                target: root
                width: launcher.itemSize + launcher.itemPadding
                height: undefined
            }
            AnchorChanges {
                target: root
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.right: undefined
                anchors.bottom: parent.bottom
            }

            PropertyChanges {
                target: leftView
                orientation: ListView.Vertical
                width: indicators.iconSize + Themes.Units.largeSpacing
                height: leftView.contentHeight
            }
            AnchorChanges {
                target: leftView
                anchors.top: root.top
                anchors.horizontalCenter: root.horizontalCenter
            }

            PropertyChanges {
                target: launcher
                orientation: ListView.Vertical
                height: root.height - leftView.height - indicatorsView.height
                viewWidth: size + (launcher.itemPadding / 2)
                viewHeight: launcher.contentHeight
            }
            AnchorChanges {
                target: launcher
                anchors.left: root.left
                anchors.top: leftView.bottom
                anchors.right: root.right
                anchors.bottom: indicatorsView.top
            }

            PropertyChanges {
                target: indicatorsView
                orientation: ListView.Vertical
                width: indicators.iconSize + Themes.Units.largeSpacing
                height: indicatorsView.contentHeight
            }
            AnchorChanges {
                target: indicatorsView
                anchors.bottom: root.bottom
                anchors.horizontalCenter: root.horizontalCenter
            }
        },
        State {
            name: "vright"

            PropertyChanges {
                target: root
                width: launcher.itemSize + launcher.itemPadding
                height: undefined
            }
            AnchorChanges {
                target: root
                anchors.left: undefined
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }

            PropertyChanges {
                target: leftView
                orientation: ListView.Vertical
                width: indicators.iconSize + Themes.Units.largeSpacing
                height: leftView.contentHeight
            }
            AnchorChanges {
                target: leftView
                anchors.top: root.top
                anchors.horizontalCenter: root.horizontalCenter
            }

            PropertyChanges {
                target: launcher
                orientation: ListView.Vertical
                height: root.height - leftView.height - indicatorsView.height
                viewWidth: size + (launcher.itemPadding / 2)
                viewHeight: launcher.contentHeight
            }
            AnchorChanges {
                target: launcher
                anchors.left: root.left
                anchors.top: leftView.bottom
                anchors.right: root.right
                anchors.bottom: indicatorsView.top
            }

            PropertyChanges {
                target: indicatorsView
                orientation: ListView.Vertical
                width: indicators.iconSize + Themes.Units.largeSpacing
                height: indicatorsView.contentHeight
            }
            AnchorChanges {
                target: indicatorsView
                anchors.bottom: root.bottom
                anchors.horizontalCenter: root.horizontalCenter
            }
        }
    ]
    transitions: Transition {
        AnchorAnimation {
            easing.type: Easing.OutQuad
            duration: Themes.Units.shortDuration
        }
    }
    onSizeChanged: {
        switch (this.size) {
        case Themes.Units.iconSizes.medium:
            launcher.itemSize = this.size * 1.20;
            indicators.iconSize = Themes.Units.iconSizes.small;
            break;
        case Themes.Units.iconSizes.large:
            launcher.itemSize = this.size * 1.25;
            indicators.iconSize = Themes.Units.iconSizes.smallMedium;
            break;
        case Themes.Units.iconSizes.huge:
            launcher.itemSize = this.size * 1.16;
            indicators.iconSize = Themes.Units.iconSizes.medium;
            break;
        default:
            break;
        }
    }

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

    ObjectModel {
        id: leftIndicators

        LauncherIndicator {
            id: launcherIndicator
        }
    }

    ObjectModel {
        property real iconSize: Themes.Units.iconSizes.smallMedium

        id: indicators

        /*
        Loader {
            asynchronous: true
            sourceComponent: ChatIndicator {
                iconSize: indicators.iconSize
                onTriggered: indicatorTriggered(caller)
            }
        }
        */

        Loader {
            asynchronous: true
            sourceComponent: EventsIndicator {
                iconSize: indicators.iconSize
                onTriggered: indicatorTriggered(caller)
            }
        }

        Loader {
            asynchronous: true
            sourceComponent: SettingsIndicator {
                iconSize: indicators.iconSize
                onTriggered: indicatorTriggered(caller)
            }
        }

        Loader {
            asynchronous: true
            sourceComponent: SoundIndicator {
                iconSize: indicators.iconSize
                onTriggered: indicatorTriggered(caller)
            }
        }

        Loader {
            asynchronous: true
            sourceComponent: NetworkIndicator {
                iconSize: indicators.iconSize
                onTriggered: indicatorTriggered(caller)
            }
        }

        Loader {
            asynchronous: true
            sourceComponent: StorageIndicator {
                iconSize: indicators.iconSize
                onTriggered: indicatorTriggered(caller)
            }
        }

        Loader {
            asynchronous: true
            sourceComponent: BatteryIndicator {
                iconSize: indicators.iconSize
                onTriggered: indicatorTriggered(caller)
            }
        }
    }

    ListView {
        id: leftView
        model: leftIndicators
        interactive: false
    }

    Launcher {
        id: launcher
        iconSize: root.size
    }

    ListView {
        id: indicatorsView
        model: indicators
        interactive: false
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
