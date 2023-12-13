// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import Fluid.Controls as FluidControls
import Liri.Shell as Shell

Rectangle {
    id: panel

    readonly property bool showing: true
    readonly property bool maximized: appsModel.hasMaximizedWindow
    readonly property bool taskBarMenuOpen: taskBar.menuOpen

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    height: 56

    color: maximized ? Material.dialogColor : "transparent"

    Behavior on color {
        ColorAnimation {
            duration: FluidControls.Units.shortDuration
        }
    }

    FluidControls.InputArea {
        region: inputRegion
        x: panel.x
        y: panel.y
        width: panel.width
        height: panel.height
    }

    Instantiator {
        model: Shell.ExtensionsModel {}
        delegate: Loader {
            property string name: model.name

            source: model.url
            onStatusChanged: {
                if (status === Loader.Error)
                    console.error("Failed to load status area extension", name);
            }
        }
        onObjectAdded: {
            if (object.name && object.item) {
                console.info("Loading status area extension:", object.name);

                if (object.item.indicator)
                    indicatorsModel.append({"component": object.item.indicator});
                if (object.item.menu)
                    menuExtensionsModel.append({"component": object.item.menu});
            }
        }
    }

    ListModel {
        id: indicatorsModel
    }

    RowLayout {
        anchors.fill: parent

        PanelButton {
            Material.elevation: panel.maximized ? 0 : 8

            onClicked: {
                launcher.open();
            }
        }

        TaskBar {
            id: taskBar

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ShelfPane {
            Material.elevation: panel.maximized ? 0 : 8
            Layout.fillHeight: true

            padding: panel.maximized ? 0 : 8

            onClicked: {
                panelMenu.open();
            }

            IndicatorsRow {
                id: indicatorsRow

                anchors.fill: parent
            }
        }
    }

    function pushToMenu(item) {
        panelMenu.pushToMenu(item);
    }

    function popFromMenu() {
        panelMenu.popFromMenu();
    }
}
