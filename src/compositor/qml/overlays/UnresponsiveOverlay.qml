// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

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
