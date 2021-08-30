// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Aurora.Client 1.0 as AuroraClient
import Fluid.Controls 1.0 as FluidControls
import Liri.Shell.TaskManager 1.0 as TaskManager
import "panel" as Panel
import "launcher" as Launcher

Window {
    id: topLayerWindow

    signal logoutRequested()
    signal lockRequested()
    signal shutdownRequested()

    color: "transparent"
    visible: false

    onVisibleChanged: {
        if (visible)
            panel.show();
        else
            panel.hide();
    }

    AuroraClient.WlrLayerSurfaceV1 {
        layer: AuroraClient.WlrLayerSurfaceV1.TopLayer
        anchors: AuroraClient.WlrLayerSurfaceV1.LeftAnchor |
                 AuroraClient.WlrLayerSurfaceV1.TopAnchor |
                 AuroraClient.WlrLayerSurfaceV1.RightAnchor |
                 AuroraClient.WlrLayerSurfaceV1.BottomAnchor
        keyboardInteractivity: AuroraClient.WlrLayerSurfaceV1.OnDemandKeyboardInteractivity
        exclusiveZone: -1
        role: "toplayer"
    }

    FluidControls.InputRegion {
        id: inputRegion

        enabled: !launcher.visible && !panelMenu.visible && !panel.taskBarMenuOpen
    }

    ListModel {
        id: menuExtensionsModel
    }

    TaskManager.ApplicationsModel {
        id: appsModel
    }

    Launcher.AppsLauncher {
        id: launcher

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        onAppLaunched: {
            launcher.close();
        }
    }

    Panel.Panel {
        id: panel

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -height

        Behavior on anchors.bottomMargin {
            NumberAnimation {
                easing.type: Easing.InOutCubic
                duration: FluidControls.Units.longDuration
            }
        }

        function show() {
            anchors.bottomMargin = 0;
        }

        function hide() {
            anchors.bottomMargin = -height;
        }
    }

    Panel.StatusMenu {
        id: panelMenu

        x: parent.width - width - FluidControls.Units.smallSpacing
        y: panel.y - height - FluidControls.Units.smallSpacing

        onLogoutRequested: {
            panelMenu.close();
            topLayerWindow.logoutRequested();
        }
        onLockRequested: {
            panelMenu.close();
            topLayerWindow.lockRequested();
        }
        onShutdownRequested: {
            panelMenu.close();
            topLayerWindow.shutdownRequested();
        }
    }
}
