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
import GreenIsland 1.0
import Hawaii.Themes 1.0 as Themes
import "indicators"
import "launcher"

Rectangle {
    property Indicator selectedIndicator: null
    property Indicator lastIndicator: null
    readonly property alias currentLauncherItem: launcher.currentItem
    property real size: Themes.Units.iconSizes.large
    property int alignment: Qt.AlignBottom
    readonly property int formFactor: (alignment == Qt.AlignLeft || alignment == Qt.AlignRight) ? Qt.Horizontal : Qt.Vertical

    signal indicatorTriggered(var indicator)

    id: root
    color: "transparent"
    height: launcher.itemSize + launcher.itemPadding
    onSizeChanged: {
        switch (size) {
        case Themes.Units.iconSizes.medium:
            indicators.iconSize = Themes.Units.iconSizes.small;
            break;
        case Themes.Units.iconSizes.large:
            indicators.iconSize = Themes.Units.iconSizes.smallMedium;
            break;
        case Themes.Units.iconSizes.huge:
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

    Connections {
        target: compositorRoot
        onActiveWindowChanged: setup()
    }

    Connections {
        target: compositorRoot.activeWindow ? compositorRoot.activeWindow.clientWindow : null
        onMaximizedChanged: setup()
    }

    RowLayout {
        anchors.fill: parent
        spacing: Themes.Units.smallSpacing

        RowLayout {
            id: leftContainer
            spacing: Themes.Units.largeSpacing

            /*
            AppChooserIndicator {
                onTriggered: indicatorTriggered(caller)
            }
            */
            LauncherIndicator {}

            Layout.alignment: Qt.AlignLeft
        }

        Launcher {
            id: launcher
            iconSize: root.size
            alignment: root.alignment

            Layout.fillWidth: true
            //Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
        }

        RowLayout {
            property int iconSize

            id: indicators
            spacing: Themes.Units.smallSpacing

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

            Layout.alignment: Qt.AlignRight
        }
    }

    function setup() {
        // TODO: Don't resize the panel, the window is maximized before we change the available
        // geometry resulting in a "hole" between the window and the panel
        if (compositorRoot.activeWindow && compositorRoot.activeWindow.clientWindow.maximized) {
            color = Themes.Theme.palette.rgba(Themes.Theme.palette.window.backgroundColor, 0.85);
            //launcher.iconSize = Themes.Units.iconSizes.medium;
        } else {
            color = "transparent";
            //launcher.iconSize = Themes.Units.iconSizes.large;
        }
    }
}
