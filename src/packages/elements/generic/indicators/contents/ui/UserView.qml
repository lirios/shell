/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtAccountsService 1.0
import Fluid.Ui 1.0 as FluidUi

Item {
    id: userView
    implicitWidth: mainLayout.width
    implicitHeight: mainLayout.height

    signal childClicked()

    QtObject {
        id: __priv

        property var palette: SystemPalette {}
        property var userAccount: UserAccount {}
        property var sessionManager: Shell.service("SessionManager")
    }

    RowLayout {
        id: mainLayout

        RowLayout {
            Image {
                id: avatarImage
                source: "file://" + __priv.userAccount.iconFileName
                sourceSize.width: width
                sourceSize.height: height
                width: 64
                height: 64
                visible: status == Image.Ready

                Layout.alignment: Qt.AlignTop
            }

            FluidUi.Icon {
                id: avatarIcon
                iconName: "avatar-default-symbolic"
                width: 64
                height: 64
                color: __priv.palette.text
                visible: !avatarImage

                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                Label {
                    text: __priv.userAccount.displayName
                    font.weight: Font.Bold
                    font.pointSize: 14
                }

                Label {
                    text: __priv.userAccount.userName
                    font.pointSize: 12
                }

                Layout.alignment: Qt.AlignTop
            }

            Layout.minimumWidth: 100
            Layout.fillHeight: true
        }

        Item {
            Layout.minimumWidth: 100
        }

        RowLayout {
            ToolButton {
                iconName: "system-switch-user"
                enabled: false
                onClicked: {
                    userView.childClicked();
                }
            }

            ToolButton {
                iconName: "system-log-out"
                onClicked: {
                    userView.childClicked();
                    __priv.sessionManager.logout();
                }
            }

            Layout.alignment: Qt.AlignTop
        }
    }
}
