/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import org.hawaii.session 1.0 as Session

Rectangle {
    gradient: Gradient {
        GradientStop { position: 0; color: Qt.darker("firebrick", 1.8) }
        GradientStop { position: 1; color: Qt.darker("firebrick", 1.1) }
    }

    ColumnLayout {
        anchors.centerIn: parent

        Label {
            text: qsTr(":(")
            color: "white"
            font.pointSize: 42

            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: qsTr("The system ran into a problem that it couldn't handle.\n" +
                       "Press the button to quit.")
            color: "white"
            font.pointSize: 24

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.minimumHeight: 100
        }

        Button {
            text: qsTr("Quit")
            onClicked: Session.SessionInterface.logOut()

            Layout.alignment: Qt.AlignHCenter
        }
    }
}
