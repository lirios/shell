/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 2.0 as PlasmaComponents
import Hawaii.Components 1.0 as Components
import "."

Item {
    signal cancel()
    signal logOutRequested()
    signal powerOffRequested()
    signal restartRequested()

    id: root
    opacity: 0.0
    state: __priv.mode
    states: [
        State {
            name: "logout"
            PropertyChanges { target: __priv; currentAction: logOutRequested }
            PropertyChanges { target: actionIcon; iconName: "system-log-out-symbolic" }
            PropertyChanges { target: actionLabel; text: qsTr("Log Out") }
            PropertyChanges { target: okButton; text: qsTr("Log out") }
        },
        State {
            name: "poweroff"
            PropertyChanges { target: __priv; currentAction: powerOffRequested }
            PropertyChanges { target: actionIcon; iconName: "system-shutdown-symbolic" }
            PropertyChanges { target: actionLabel; text: qsTr("Power Off") }
            PropertyChanges { target: okButton; text: qsTr("Power off") }
        },
        State {
            name: "restart"
            PropertyChanges { target: __priv; currentAction: restartRequested }
            PropertyChanges { target: actionIcon; iconName: "system-reboot-symbolic" }
            PropertyChanges { target: actionLabel; text: qsTr("Restart") }
            PropertyChanges { target: okButton; text: qsTr("Restart") }
        }
    ]

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InSine
            duration: units.longDuration
        }
    }

    QtObject {
        id: __priv

        property string mode: "poweroff"
        property real timeout: 60
        property real remainingTime: timeout
        property var currentAction

        onModeChanged: remainingTime = timeout
        onRemainingTimeChanged: {
            if (remainingTime <= 0)
                currentAction();
        }
    }

    Timer {
        running: true
        repeat: true
        interval: 10000
        onTriggered: __priv.remainingTime -= 10
    }

    Image {
        id: picture
        anchors.fill: parent
        source: "/usr/share/backgrounds/gnome/Waves.jpg"
        sourceSize.width: width * 0.75
        sourceSize.height: height * 0.75
        fillMode: Image.PreserveAspectCrop
        cache: false
        visible: false
    }

    FastBlur {
        anchors.fill: picture
        source: picture
        radius: 32

        // Catch mouse InputMethod
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
        }
    }

    ColumnLayout {
        anchors.centerIn: parent

        Components.Icon {
            id: actionIcon
            width: units.iconSizes.huge
            height: width
            color: Theme.panel.textColor

            MouseArea {
                anchors.fill: parent
                onClicked: __priv.currentAction()
            }

            Layout.alignment: Qt.AlignHCenter
        }

        PlasmaExtras.Heading {
            id: actionLabel
            level: 2
            color: Theme.panel.textColor

            Layout.alignment: Qt.AlignHCenter
        }

        ProgressBar {
            width: units.largeSpacing * 5
            minimumValue: 0
            maximumValue: __priv.timeout
            value: __priv.remainingTime

            Layout.alignment: Qt.AlignHCenter
        }

        PlasmaComponents.Label {
            id: actionText
            text: {
                var msg = "";

                switch (root.state) {
                case "logout":
                    msg = "You will be logged out automatically in %1 seconds.";
                    break;
                case "poweroff":
                    msg = "The system will power off automatically in %1 seconds.";
                    break;
                case "restart":
                    msg = "The system will restart automatically in %1 seconds.";
                    break;
                default:
                    break;
                }

                return i18nd("org.hawaii.desktop", msg, __priv.remainingTime);
            }
            color: Theme.panel.textColor

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            height: units.largeSpacing * 2
        }



        Item {
            height: units.largeSpacing * 2
        }

        RowLayout {
            Button {
                text: qsTr("Cancel")
                onClicked: root.cancel()
            }

            Button {
                id: okButton
                isDefault: true
                onClicked: __priv.currentAction()
            }

            Layout.alignment: Qt.AlignHCenter
        }
    }

    Component.onCompleted: opacity = 1.0
}
