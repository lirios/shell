// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Liri.WaylandClient 1.0 as WaylandClient
import Fluid.Controls 1.0 as FluidControls

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
        anchors: WaylandClient.WlrLayerSurfaceV1.TopAnchor |
                 WaylandClient.WlrLayerSurfaceV1.RightAnchor
        keyboardInteractivity: WaylandClient.WlrLayerSurfaceV1.NoKeyboardInteractivity
        topMargin: FluidControls.Units.smallSpacing
        rightMargin: FluidControls.Units.smallSpacing
        role: "notification"
    }

    Rectangle {
        id: container

        Material.theme: Material.Dark
        Material.primary: Material.Blue
        Material.accent: Material.Blue

        anchors.left: parent.left
        anchors.top: parent.top

        color: Material.dialogColor
        radius: 2

        implicitWidth: FluidControls.Units.gu(24)
        implicitHeight: layout.implicitHeight + FluidControls.Units.smallSpacing

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

            FluidControls.ThinDivider {
                visible: actionsModel.count > 0
            }

            RowLayout {
                id: actionsContainer

                visible: actionsModel.count > 0

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

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
}
