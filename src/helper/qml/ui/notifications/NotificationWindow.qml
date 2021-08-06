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

    property int notificationId
    property string appName
    property string appIcon
    property alias iconUrl: imageItem.source
    property alias hasIcon: imageItem.visible
    property alias summary: titleLabel.text
    property alias body: bodyLabel.text
    readonly property alias actionsModel: actionsModel
    property bool isPersistent: false
    property alias expireTimeout: timer.interval
    property var hints

    signal actionInvoked(string actionId)

    screen: primaryScreen
    color: "transparent"
    width: control.width + (closeButton.width / 2) + (control.Material.elevation * 4)
    height: control.height + (closeButton.height / 2) + (control.Material.elevation * 4)
    visible: true

    Timer {
        id: timer

        interval: 5000
        running: !isPersistent
        onTriggered: {
            if (!isPersistent) {
                timer.running = false;
                notificationWindow.close();
            }
        }
    }

    WaylandClient.WlrLayerSurfaceV1 {
        layer: WaylandClient.WlrLayerSurfaceV1.TopLayer
        anchors: WaylandClient.WlrLayerSurfaceV1.BottomAnchor |
                 WaylandClient.WlrLayerSurfaceV1.RightAnchor
        keyboardInteractivity: WaylandClient.WlrLayerSurfaceV1.NoKeyboardInteractivity
        bottomMargin: FluidControls.Units.smallSpacing
        rightMargin: FluidControls.Units.smallSpacing
        role: "notification"
    }

    ShellHelper.WindowMask {
        id: windowMask
    }

    RoundButton {
        id: closeButton

        Material.theme: Material.Dark
        Material.background: control.Material.dialogColor
        Material.elevation: 0

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: control.Material.elevation * 2
        anchors.rightMargin: control.Material.elevation * 2

        icon.source: FluidControls.Utils.iconUrl("navigation/close")

        z: 1

        onClicked: {
            notificationWindow.close();
        }
    }

    Page {
        id: control

        Material.theme: Material.Dark
        Material.primary: Material.Blue
        Material.accent: Material.Blue
        Material.elevation: 8

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: control.Material.elevation * 2
        anchors.topMargin: (closeButton.height / 2) + (control.Material.elevation * 2)

        implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                                contentWidth + leftPadding + rightPadding,
                                implicitHeaderWidth,
                                implicitFooterWidth)
        implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                                 contentHeight + topPadding + bottomPadding
                                 + (implicitHeaderHeight > 0 ? implicitHeaderHeight + spacing : 0)
                                 + (implicitFooterHeight > 0 ? implicitFooterHeight + spacing : 0))

        contentWidth: FluidControls.Units.gu(24)

        Behavior on opacity {
            NumberAnimation { duration: 150 }
        }

        background: Rectangle {
            radius: 2
            color: control.Material.dialogColor

            layer.enabled: true
            layer.effect: FluidEffects.Elevation {
                elevation: control.Material.elevation
            }
        }

        ColumnLayout {
            id: layout

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: FluidControls.Units.smallSpacing

            spacing: FluidControls.Units.smallSpacing

            RowLayout {
                spacing: FluidControls.Units.smallSpacing

                Image {
                    id: imageItem

                    width: FluidControls.Units.iconSizes.large
                    height: width
                    sourceSize.width: width
                    sourceSize.height: height
                    fillMode: Image.PreserveAspectFit
                    cache: false
                    smooth: true
                    visible: false

                    Layout.alignment: Qt.AlignTop
                }

                ColumnLayout {
                    spacing: FluidControls.Units.smallSpacing

                    FluidControls.SubheadingLabel {
                        id: titleLabel

                        font.weight: Font.Bold
                        elide: Text.ElideRight
                        visible: text.length > 0
                        onLinkActivated: {
                            Qt.openUrlExternally(link);
                        }
                    }

                    FluidControls.BodyLabel {
                        id: bodyLabel

                        color: Material.secondaryTextColor
                        wrapMode: Text.Wrap
                        elide: Text.ElideRight
                        maximumLineCount: 10
                        verticalAlignment: Text.AlignTop
                        visible: text.length > 0
                        onLinkActivated: {
                            Qt.openUrlExternally(link);
                        }

                        Layout.fillWidth: true
                    }
                }
            }

            Item {
                height: FluidControls.Units.smallSpacing
            }
        }

        footer: ColumnLayout {
            spacing: FluidControls.Units.smallSpacing

            FluidControls.ThinDivider {
                visible: actionsModel.count > 0
            }

            RowLayout {
                id: actionsContainer

                visible: actionsModel.count > 0

                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.bottomMargin: FluidControls.Units.smallSpacing

                Repeater {
                    id: actionsRepeater

                    model: ListModel {
                        id: actionsModel
                    }

                    Button {
                        text: model.text
                        flat: true
                        onClicked: {
                            notificationWindow.actionInvoked(model.id);
                        }
                    }
                }
            }
        }
    }

    // Do not capture click events, just change opacity when the pointer
    // is moved. This will make stuff underneath it visible.
    // Areas with buttons are not sensitive to mouse hover.
    MouseArea {
        x: control.x
        y: control.y
        width: control.implicitWidth
        height: control.implicitHeight - control.implicitFooterHeight
        acceptedButtons: Qt.NoButton
        hoverEnabled: true

        onContainsMouseChanged: {
            control.opacity = containsMouse ? 0.5 : 1.0;
        }

        Component.onCompleted: {
            windowMask.addRect(Qt.rect(x, y, width, height));
        }
    }
}
