// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Fluid.Controls 1.0 as FluidControls
import Liri.Shell 1.0 as Shell
import Liri.Shell.TaskManager 1.0 as TaskManager
import Liri.Shell.Notifications 1.0 as NotificationServer
import "panel" as Panel
import "launcher" as Launcher
import "notifications" as Notifications

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

    Shell.WlrLayerSurfaceV1 {
        layer: Shell.WlrLayerSurfaceV1.TopLayer
        anchors: Shell.WlrLayerSurfaceV1.LeftAnchor |
                 Shell.WlrLayerSurfaceV1.TopAnchor |
                 Shell.WlrLayerSurfaceV1.RightAnchor |
                 Shell.WlrLayerSurfaceV1.BottomAnchor
        keyboardInteractivity: Shell.WlrLayerSurfaceV1.OnDemandKeyboardInteractivity
        exclusiveZone: -1
        role: "toplayer"
    }

    FluidControls.InputRegion {
        id: inputRegion

        enabled: !launcher.visible && !panelMenu.visible && !panel.taskBarMenuOpen
    }

    FluidControls.InputArea {
        region: inputRegion
        x: notificationsListView.x
        y: notificationsListView.y
        width: notificationsListView.width
        height: notificationsListView.height
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

    /*
     * Notifications
     */

    ListView {
        id: notificationsListView

        anchors.top: parent.top
        anchors.bottom: panel.top
        anchors.right: parent.right

        verticalLayoutDirection: ListView.BottomToTop
        spacing: FluidControls.Units.smallSpacing

        // width = 24gu + (closeButton.width / 2) + (elevation * 4)
        width: FluidControls.Units.gu(24) + (48 / 2) + (8 * 4)

        model: NotificationServer.NotificationsModel {
            id: notificationsModel
        }

        delegate: Notifications.NotificationItem {
            notificationId: model.notificationId
            appName: model.appName
            appIcon: model.appIcon
            iconUrl: model.iconUrl
            hasIcon: model.hasIcon
            summary: model.summary
            body: model.body
            isPersistent: model.isPersistent
            expireTimeout: model.expireTimeout
            hints: model.hints
            actions: model.actions

            onActionInvoked: {
                notificationsModel.invokeAction(notificationId, actionId);
            }
            onClosed: {
                notificationsModel.closeNotification(notificationId);
            }
        }

        add: Transition {
            NumberAnimation {
                properties: "x,y"
                easing.type: Easing.OutQuad
                duration: 220
            }
        }
        remove: Transition {
            NumberAnimation {
                properties: "x,y"
                easing.type: Easing.OutQuad
                duration: 220
            }
        }
        populate: Transition {
            NumberAnimation {
                properties: "x,y"
                easing.type: Easing.OutQuad
                duration: 220
            }
        }
        displaced: Transition {
            NumberAnimation {
                properties: "x,y"
                easing.type: Easing.OutQuad
                duration: 220
            }
        }
    }

    /*
     * Panel
     */

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
