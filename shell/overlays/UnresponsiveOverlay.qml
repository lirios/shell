/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

Colorize {
    property alias window: root.source

    id: root
    hue: 0.0
    saturation: 0.5
    lightness: -0.5
    z: 1

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.Linear
            duration: 250
        }
    }

    // Automatically ping the surface
    Timer {
        running: window.unresponsive
        interval: 10000
        onTriggered: window.pingSurface()
    }

    // Block input on the surface
    MouseArea {
        anchors.fill: parent
        enabled: window.unresponsive
        cursorShape: Qt.BusyCursor
        z: 1
    }

    ColumnLayout {
        anchors.fill: parent
        enabled: window.unresponsive
        z: 2

        Item {
            Layout.fillHeight: true
        }

        Text {
            font.bold: true
            font.pointSize: 14
            color: "#f1f1f1"
            text: qsTr("Application is not responding")
            horizontalAlignment: Qt.AlignHCenter

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
        }

        Text {
            font.pointSize: 10
            color: "#f1f1f1"
            text: qsTr("This application is unresponsive, you may choose to wait " +
                       "a short while for it to continue or force the application to " +
                       "quit entirely.")
            horizontalAlignment: Qt.AlignHCenter
            wrapMode: Text.Wrap

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }

        RowLayout {
            Button {
                text: qsTr("Wait")
                onClicked: window.pingSurface()
            }

            Button {
                text: qsTr("Force Quit")
                onClicked: window.child.surface.client.kill()
            }

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
