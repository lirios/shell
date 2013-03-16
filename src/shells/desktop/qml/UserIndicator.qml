/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import GreenIsland 1.0
import FluidUi 1.0
import QtAccountsService 5.0

PanelIndicator {
    property variant userAccount: UserAccount {}
    property int userStatus: UserStatus.Offline

    iconName: userStatusIcon(userStatus)
    label: userAccount.displayName
    menu: PanelMenu {
        content: [
            PanelContainerMenuItem {
                items: [
                    Image {
                        id: avatarImage
                        source: "image://desktoptheme/avatar-default-symbolic"
                        sourceSize: Qt.size(theme.largeIconSize, theme.largeIconSize)
                    },
                    Column {
                        Label {
                            id: userLabel
                            text: userAccount.displayName
                            font.weight: Font.Bold
                            font.pointSize: 16
                        }

                        Row {
                            Image {
                                source: "image://desktoptheme/" + iconName
                                sourceSize: Qt.size(theme.smallIconSize, theme.smallIconSize)
                            }
                            Label {
                                text: userStatusText(userStatus)
                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                ]
            },
            PanelMenuItem {
                text: qsTr("Disconnect")
                onClicked: shell.disconnectUser()
            }
        ]
    }

    function userStatusIcon(status) {
        switch (status) {
        case UserStatus.Offline:
            return "user-offline-symbolic";
        case UserStatus.Available:
            return "user-available-symbolic";
        case UserStatus.Busy:
            return "user-busy-symbolic";
        case UserStatus.Invisible:
            return "user-invisible-symbolic";
        case UserStatus.Away:
            return "user-away-symbolic";
        case UserStatus.Idle:
            return "user-idle-symbolic";
        case UserStatus.Pending:
            return "user-status-pending-symbolic";
        case UserStatus.Locked:
            return "changes-prevent-symbolic";
        }
    }

    function userStatusText(status) {
        switch (status) {
        case UserStatus.Offline:
            return qsTr("Offline");
        case UserStatus.Available:
            return qsTr("Available");
        case UserStatus.Busy:
            return qsTr("Busy");
        case UserStatus.Invisible:
            return qsTr("Invisible");
        case UserStatus.Away:
            return qsTr("Away");
        case UserStatus.Idle:
            return qsTr("Idle");
        case UserStatus.Pending:
            return qsTr("Pending");
        case UserStatus.Locked:
            return qsTr("Locked");
        }
    }
}
