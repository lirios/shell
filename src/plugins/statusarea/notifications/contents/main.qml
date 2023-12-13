// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Fluid.Controls as FluidControls
import Liri.Shell as Shell
import Liri.Notifications as Notifications

Shell.StatusAreaExtension {
    readonly property bool silentMode: false

    ListModel {
        id: notificationsModel
    }

    Connections {
        target: Notifications.NotificationsService

        function onNotificationReceived(notificationId, appName, appIcon,
                                        hasIcon, summary, body, actions,
                                        isPersistent, expireTimeout, hints) {
            // Replace existing notification
            for (var i = 0; i < notificationsModel.count; i++) {
                var notification = notificationsModel.get(i)
                if (notification.id === notificationId) {
                    notification.summary = summary
                    notification.body = body
                    notification.actions = actions
                    notification.hints = hints
                    return
                }
            }

            // Add new notifications
            notificationsModel.append({"id": notificationId, "appName": appName,
                                          "appIcon": appIcon, "hasIcon": hasIcon,
                                          "summary": summary, "body": body,
                                          "actions": actions, "isPersistent": isPersistent,
                                          "expireTimeout": expireTimeout, "hints": hints});

        }
    }

    Component {
        id: pageComponent

        Page {
            padding: 0
            header: RowLayout {
                ToolButton {
                    icon.source: FluidControls.Utils.iconUrl("navigation/arrow_back")
                    onClicked: {
                        popFromMenu();
                    }
                }

                FluidControls.TitleLabel {
                    text: qsTr("Notifications")

                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                }
            }

            FluidControls.Placeholder {
                anchors.centerIn: parent

                icon.source: FluidControls.Utils.iconUrl("social/notifications_none")
                text: qsTr("No notifications")
                visible: notificationView.count == 0
            }

            ScrollView {
                anchors.fill: parent
                clip: true

                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                ListView {
                    id: notificationView

                    spacing: FluidControls.Units.largeSpacing
                    model: notificationsModel
                    visible: count > 0
                    delegate: FluidControls.ListItem {
                        property int notificationId: model.id

                        icon.name: model.appIcon ? model.appIcon : ""
                        icon.source: model.appIcon ? "" : model.hasIcon ? "image://notifications/%1/%2".arg(model.id).arg(Date.now() / 1000 | 0) : FluidControls.Utils.iconUrl("social/notifications")
                        text: model.summary
                        onClicked: {
                            for (var i = 0; i < notificationsModel.count; i++) {
                                var notification = notificationsModel.get(i)
                                if (notification.id === notificationId) {
                                    notificationsModel.remove(i)
                                    break
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    indicator: Shell.Indicator {
        title: qsTr("Notifications")
        iconSource: FluidControls.Utils.iconUrl(
                        silentMode ? "social/notifications_off"
                                   : notificationsModel.count > 0 ? "social/notifications"
                                                                  : "social/notifications_none"
                        )
    }

    menu: FluidControls.ListItem {
        icon.source: FluidControls.Utils.iconUrl(
                         silentMode ? "social/notifications_off"
                                    : notificationsModel.count > 0 ? "social/notifications"
                                                                   : "social/notifications_none"
                         )
        text: qsTr("Notifications")
        onClicked: {
            pushToMenu(pageComponent);
        }
    }
}
