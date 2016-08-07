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

Rectangle {
    id: panel

    readonly property alias launcherIndicator: launcherIndicator
    readonly property alias currentLauncherItem: launcher.currentItem

    property real size: 56
    property color darkColor: Material.dialogColor

    property bool showing
    property bool maximized: screenView.hasMaximizedWindow

    color: maximized ? darkColor : "transparent"

    signal indicatorTriggered(var indicator)

    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
        bottomMargin: showing ? 0 : -height

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

    /*
     * Layout
     */

    RowLayout {
        anchors.fill: parent

        Item {
            Layout.fillHeight: true
            Layout.preferredWidth: height

            Rectangle {
                anchors {
                    fill: parent
                    margins: panel.maximized ? 0 : 8
                }

                radius: 2
                color: panel.darkColor
                layer.enabled: !panel.maximized
                layer.effect: ElevationEffect {
                    elevation: 8
                }

                LauncherIndicator {
                    id: launcherIndicator
                    width: height
                }
            }
        }

        Launcher {
            id: launcher

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            Layout.fillHeight: true
            Layout.preferredWidth: indicatorsRect.implicitWidth + indicatorsRect.anchors.margins * 2

            Rectangle {
                id: indicatorsRect

                anchors {
                    fill: parent
                    margins: panel.maximized ? 0 : 8
                }

                implicitWidth: indicatorsView.width + 2 * Units.smallSpacing

                radius: 2
                color: panel.darkColor
                layer.enabled: !panel.maximized
                layer.effect: ElevationEffect {
                    elevation: 8
                }

                Row {
                    id: indicatorsView

                    anchors.centerIn: parent

                    height: parent.height

                    DateTimeIndicator {
                        onClicked: {
                            indicatorTriggered(caller)
                        }
                    }

                    EventsIndicator {
                        onClicked: indicatorTriggered(caller)
                    }

                    SettingsIndicator {
                        onClicked: indicatorTriggered(caller)
                    }

                    SoundIndicator {
                        onClicked: indicatorTriggered(caller)
                    }

                    // FIXME: This crashes the shell
                    // NetworkIndicator {
                    //     onClicked: indicatorTriggered(caller)
                    // }

                    StorageIndicator {
                        onClicked: indicatorTriggered(caller)
                    }

                    BatteryIndicator {
                        onClicked: indicatorTriggered(caller)
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        setAvailableGeometry()
        showing = true
    }

    function setAvailableGeometry() {
        output.availableGeometry = Qt.rect(0, 0, output.geometry.width, output.geometry.height - height -
                                           2 * panel.anchors.margins)
    }
}
