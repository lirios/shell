/****************************************************************************
 * This file is part of Hawaii Shell.
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

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Shell 1.0
import "."

Item {
    id: shutdownView
    implicitWidth: mainLayout.width
    implicitHeight: mainLayout.height

    signal childClicked()

    QtObject {
        id: __priv

        property var dateTime: Shell.service("DateTime")
        property var sessionManager: Shell.service("SessionManager")
    }

    RowLayout {
        id: mainLayout

        Label {
            id: dateTimeLabel
            wrapMode: Text.Wrap
            maximumLineCount: 2

            //Layout.maximumWidth: 100
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
        }

        Connections {
            target: __priv.dateTime
            onDateTimeChanged: dateTimeLabel.text = Qt.formatDate(__priv.dateTime.dateTime, Qt.DefaultLocaleLongDate)
        }

        RowLayout {
            ToolButton {
                iconName: "preferences-system-symbolic"
                onClicked: {
                    shutdownView.childClicked();
                }
            }

            ToolButton {
                iconName: "changes-prevent-symbolic"
                onClicked: {
                    shutdownView.childClicked();
                    __priv.sessionManager.lock();
                }
            }

            ToolButton {
                iconName: "system-shutdown-symbolic"
                onClicked: {
                    shutdownView.childClicked();
                    DialogsSingleton.shutdownDialog.visible = true;
                }
            }

            ToolButton {
                iconName: "media-playback-pause-symbolic"
                visible: false
                onClicked: {
                    shutdownView.childClicked();
                }
            }

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
        }
    }
}
