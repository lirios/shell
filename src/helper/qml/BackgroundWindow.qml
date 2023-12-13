// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtGSettings 1.0 as Settings
import Fluid.Effects 1.0 as FluidEffects
import Liri.DBusService 1.0 as DBusService
import Liri.Shell 1.0 as Shell

Window {
    id: bgWindow

    color: Material.color(Material.Grey, Material.Shade700)
    visible: true

    Shell.WlrLayerSurfaceV1 {
        layer: Shell.WlrLayerSurfaceV1.BackgroundLayer
        anchors: Shell.WlrLayerSurfaceV1.TopAnchor |
                 Shell.WlrLayerSurfaceV1.BottomAnchor |
                 Shell.WlrLayerSurfaceV1.LeftAnchor |
                 Shell.WlrLayerSurfaceV1.RightAnchor
        keyboardInteractivity: Shell.WlrLayerSurfaceV1.NoKeyboardInteractivity
        exclusiveZone: -1
        role: "background"
    }

    Settings.GSettings {
        id: bgSettings

        schema.id: "io.liri.desktop.background"
        schema.path: "/io/liri/desktop/background/"
    }

    DBusService.Application {
        id: settingsApp

        serviceName: "io.liri.Settings"
        objectPath: "/io/liri/Settings"
    }

    Menu {
        id: bgMenu

        MenuItem {
            text: qsTr("Change Background...")
            onClicked: settingsApp.open(["background"])
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Display Settings")
            onClicked: settingsApp.open(["display"])
        }

        MenuItem {
            text: qsTr("Settings")
            onClicked: settingsApp.activate()
        }
    }

    Shell.Background {
        id: background

        anchors.fill: parent

        pictureWidth: bgWindow.width
        pictureHeight: bgWindow.height

        // All the necessary for the "present" mode
        layer.enabled: false
        layer.effect: FluidEffects.Elevation {
            elevation: 24
        }

        state: "normal"
        states: [
            State {
                name: "normal"

                PropertyChanges {
                    target: background
                    anchors.margins: 0
                }
            },
            State {
                name: "present"

                // Margins respect screen aspect ratio
                PropertyChanges {
                    target: background
                    anchors.leftMargin: bgWindow.width * 0.1
                    anchors.rightMargin: bgWindow.width * 0.1
                    anchors.topMargin: bgWindow.height * 0.1
                    anchors.bottomMargin: bgWindow.height * 0.1
                }
            }

        ]
        transitions: [
            Transition {
                to: "normal"

                SequentialAnimation {
                    NumberAnimation {
                        properties: "anchors.leftMargin,anchors.rightMargin,anchors.topMargin,anchors.bottomMargin"
                        easing.type: Easing.OutQuad
                        duration: 300
                    }

                    ScriptAction { script: background.layer.enabled = false }
                }
            },
            Transition {
                to: "present"

                SequentialAnimation {
                    ScriptAction { script: background.layer.enabled = true }

                    NumberAnimation {
                        properties: "anchors.leftMargin,anchors.rightMargin,anchors.topMargin,anchors.bottomMargin"
                        easing.type: Easing.InQuad
                        duration: 300
                    }
                }
            }
        ]

        mode: bgSettings.mode
        pictureUrl: bgSettings.pictureUrl
        primaryColor: bgSettings.primaryColor
        secondaryColor: bgSettings.secondaryColor
        fillMode: bgSettings.fillMode
        blur: false
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: bgMenu.popup(mouse.x, mouse.y)
        onPressAndHold: bgMenu.popup(mouse.x, mouse.y)
    }
}
