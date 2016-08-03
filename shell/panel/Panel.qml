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
import Fluid.Controls 1.0
import Fluid.Material 1.0
import "../indicators"
import "../launcher"

Item {
    id: panel

    readonly property alias launcherIndicator: launcherIndicator
    readonly property alias currentLauncherItem: launcher.currentItem

    property real size: 56
    property color color: Material.dialogColor

    property bool showing

    signal indicatorTriggered(var indicator)

    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
        bottomMargin: showing ? Units.smallSpacing : -height
        margins: Units.smallSpacing

        Behavior on bottomMargin {
            NumberAnimation {
                easing.type: Easing.InOutCubic
                duration: Units.mediumDuration
            }
        }
    }

    height: size

    onWidthChanged: setAvailableGeometry()
    onHeightChanged: setAvailableGeometry()

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Units.shortDuration
        }
    }

    /*
     * Focused window
     */

    // FIXME: The following two connections don't work

    Connections {
        target: compositor.applicationManager
        onFocusedWindowChanged: setup()
    }

    Connections {
        target: compositor.applicationManager.focusedWindow
        onMaximizedChanged: setup()
    }

    /*
     * Layout
     */

    RowLayout {
        anchors.fill: parent

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: launcherIndicator.width

            radius: 2
            color: panel.color
            layer.enabled: true
            layer.effect: ElevationEffect {
                elevation: 8
            }

            LauncherIndicator {
                id: launcherIndicator
                width: height
            }
        }

        Launcher {
            id: launcher

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: indicatorsView.implicitWidth + Units.smallSpacing * 2

            radius: 2
            color: panel.color
            layer.enabled: true
            layer.effect: ElevationEffect {
                elevation: 8
            }

            RowLayout {
                id: indicatorsView

                anchors.centerIn: parent

                spacing: 0
                height: parent.height

                property real iconSize: 24

                DateTimeIndicator {
                    iconSize: indicatorsView.iconSize
                    onClicked: {
                        console.log("CLICKED", caller)
                        indicatorTriggered(caller)
                    }
                }

                EventsIndicator {
                    iconSize: indicatorsView.iconSize
                    onClicked: indicatorTriggered(caller)
                }

                SettingsIndicator {
                    iconSize: indicatorsView.iconSize
                    onClicked: indicatorTriggered(caller)
                }

                SoundIndicator {
                    iconSize: indicatorsView.iconSize
                    onClicked: indicatorTriggered(caller)
                }

                // FIXME: This crashes the shell
                // NetworkIndicator {
                //     iconSize: indicatorsView.iconSize
                //     onClicked: indicatorTriggered(caller)
                // }

                StorageIndicator {
                    iconSize: indicatorsView.iconSize
                    onClicked: indicatorTriggered(caller)
                }

                BatteryIndicator {
                    iconSize: indicatorsView.iconSize
                    onClicked: indicatorTriggered(caller)
                }
            }
        }
    }

    Component.onCompleted: {
        setAvailableGeometry()
        showing = true
    }

    function setup() {
        // TODO: Don't resize the panel, the window is maximized before we change the available
        // geometry resulting in a "hole" between the window and the panel
        var window = compositor.applicationManager.focusedWindow;
        if (window && window.maximized) {
            color = Utils.alpha(Material.dialogColor, 0.9);
            //launcher.iconSize = Units.iconSizes.medium;
        } else {
            color = "transparent";
            //launcher.iconSize = Units.iconSizes.large;
        }
    }

    function setAvailableGeometry() {
        output.availableGeometry = Qt.rect(0, 0, output.geometry.width, output.geometry.height - height -
                                           2 * panel.anchors.margins)
    }
}
