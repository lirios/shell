/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15 as MaterialImpl
import QtQuick.Layouts 1.15
import QtQml.Models 2.2
import Fluid.Controls 1.0
import Fluid.Effects 1.0 as FluidEffects
import Liri.Shell 1.0
import "../indicators"
import "../launcher"

Rectangle {
    id: panel

    readonly property alias launcherIndicator: launcherIndicator
    readonly property alias currentLauncherItem: launcher.currentItem
    readonly property int rightWidth: indicatorsRect.implicitWidth + indicatorsRect.anchors.margins * 2

    property real size: 56
    property color darkColor: Material.dialogColor

    property bool showing: !liriCompositor.hasFullscreenShellSurfaces
    property bool maximized: liriCompositor.hasMaxmizedShellSurfaces

    color: maximized ? darkColor : "transparent"

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

    Behavior on color {
        ColorAnimation {
            duration: Units.shortDuration
        }
    }

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: Units.shortDuration
        }
    }

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
                layer.effect: FluidEffects.Elevation {
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
                    margins: panel.maximized ? 0 : Units.smallSpacing
                }

                implicitWidth: indicatorsView.width + 2 * Units.smallSpacing

                radius: 2
                color: panel.darkColor
                layer.enabled: !panel.maximized
                layer.effect: FluidEffects.Elevation {
                    elevation: 8
                }

                TapHandler {
                    id: tapHandler

                    onTapped: {
                        panelMenu.open();
                    }
                }

                MaterialImpl.Ripple {
                    anchors.fill: parent
                    clip: visible
                    pressed: tapHandler.pressed
                    anchor: indicatorsRect
                    color: Material.rippleColor
                }

                IndicatorsRow {
                    id: indicatorsView

                    anchors.centerIn: parent
                }

                Rectangle {
                    id: highlight

                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }

                    height: 2
                    color: Material.accentColor
                    opacity: panelMenu.visible ? 1 : 0

                    Behavior on opacity {
                        NumberAnimation {
                            duration: Units.mediumDuration
                        }
                    }

                    Behavior on color {
                        ColorAnimation {
                            duration: Units.mediumDuration
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        setAvailableGeometry();
    }

    function setAvailableGeometry() {
        output.availableGeometry =
                Qt.rect(0, 0,
                        outputWindow.width,
                        outputWindow.height - panel.height - (2 * panel.anchors.margins));
    }
}
