// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Liri.WaylandClient 1.0 as WaylandClient
import Liri.ShellHelper 1.0 as ShellHelper
import Fluid.Controls 1.0 as FluidControls
import Fluid.Effects 1.0 as FluidEffects

Window {
    id: notificationWindow

    property alias model: notificationsModel

    screen: primaryScreen
    color: "transparent"
    // width = 24gu + (closeButton.width / 2) + (elevation * 4)
    width: FluidControls.Units.gu(24) + (48 / 2) + (8 * 4)
    visible: true

    WaylandClient.WlrLayerSurfaceV1 {
        layer: WaylandClient.WlrLayerSurfaceV1.TopLayer
        anchors: WaylandClient.WlrLayerSurfaceV1.RightAnchor |
                 WaylandClient.WlrLayerSurfaceV1.TopAnchor |
                 WaylandClient.WlrLayerSurfaceV1.BottomAnchor
        keyboardInteractivity: WaylandClient.WlrLayerSurfaceV1.NoKeyboardInteractivity
        leftMargin: FluidControls.Units.smallSpacing
        topMargin: FluidControls.Units.smallSpacing
        rightMargin: FluidControls.Units.smallSpacing
        bottomMargin: FluidControls.Units.smallSpacing
        role: "notifications"
    }

    ShellHelper.InputRegion {
        rects: ShellHelper.Rect {
            width: listView.count > 0 ? notificationWindow.width : 1
            height: listView.count > 0 ? notificationWindow.height : 1
        }
    }

    ListView {
        id: listView

        anchors.fill: parent
        verticalLayoutDirection: ListView.BottomToTop
        spacing: FluidControls.Units.smallSpacing

        model: ListModel {
            id: notificationsModel
        }

        delegate: NotificationItem {
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

            onClosed: {
                notificationsModel.remove(index);
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
}
