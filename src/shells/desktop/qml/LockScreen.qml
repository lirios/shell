/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Shell.Desktop 0.1

Item {
    SessionManager {
        id: sessionManager
    }

    Background {
        anchors.fill: parent

        ColumnLayout {
            anchors.centerIn: parent

            Text {
                text: Qt.formatDate(new Date(), Qt.locale().dateFormat(Locale.LongFormat))
                font.pointSize: 48
                color: "white"
                style: Text.Raised
                styleColor: Qt.rgba(0, 0, 0, 0.7)
            }

            Item {
                Layout.fillHeight: true
            }

            Button {
                text: qsTr("Unlock")
                onClicked: sessionManager.unlock()

                Layout.alignment: Qt.AlignCenter
            }
        }
    }
}
