/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

import QtQuick 2.4
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0

Showable {
    property alias mode: __priv.mode

    signal cancel()
    signal logOutRequested()
    signal powerOffRequested()
    signal restartRequested()
    signal suspendRequested()
    signal hibernateRequested()

    id: root
    opacity: 0.0
    showAnimation: OpacityAnimator {
        target: root
        easing.type: Easing.InSine
        duration: Units.longDuration
        from: 0.0
        to: 1.0
    }
    hideAnimation: OpacityAnimator {
        target: root
        easing.type: Easing.InSine
        duration: Units.longDuration
        from: 1.0
        to: 0.0
    }
    state: __priv.mode
    states: [
        State {
            name: "logout"
            PropertyChanges { target: __priv; currentAction: logOutRequested }
            PropertyChanges { target: actionIcon; name: Qt.resolvedUrl("../images/logout.svg") }
            PropertyChanges { target: actionLabel; text: qsTr("Log Out") }
            PropertyChanges { target: okButton; text: qsTr("Log out") }
        },
        State {
            name: "poweroff"
            PropertyChanges { target: __priv; currentAction: powerOffRequested }
            PropertyChanges { target: actionIcon; name: "action/power_settings_new" }
            PropertyChanges { target: actionLabel; text: qsTr("Power Off") }
            PropertyChanges { target: okButton; text: qsTr("Power off") }
        },
        State {
            name: "restart"
            PropertyChanges { target: __priv; currentAction: restartRequested }
            PropertyChanges { target: actionIcon; name: Qt.resolvedUrl("../images/reload.svg") }
            PropertyChanges { target: actionLabel; text: qsTr("Restart") }
            PropertyChanges { target: okButton; text: qsTr("Restart") }
        },
        State {
            name: "suspend"
            PropertyChanges { target: __priv; currentAction: suspendRequested }
            PropertyChanges { target: actionIcon; name: Qt.resolvedUrl("../images/sleep.svg") }
            PropertyChanges { target: actionLabel; text: qsTr("Suspend") }
            PropertyChanges { target: okButton; text: qsTr("Suspend") }
        },
        State {
            name: "hibernate"
            PropertyChanges { target: __priv; currentAction: hibernateRequested }
            PropertyChanges { target: actionIcon; name: Qt.resolvedUrl("../images/hibernate.svg") }
            PropertyChanges { target: actionLabel; text: qsTr("Hibernate") }
            PropertyChanges { target: okButton; text: qsTr("Hibernate") }
        }
    ]
    visible: true
    onLogOutRequested: SessionInterface.logOut()
    onPowerOffRequested: SessionInterface.powerOff()
    onRestartRequested: SessionInterface.restart()
    onSuspendRequested: SessionInterface.suspend()
    onHibernateRequested: SessionInterface.hibernate()

    Material.theme: Material.Dark

    QtObject {
        id: __priv

        property string mode: "poweroff"
        property int timeout: 30
        property int countDown: 10
        property int remainingTime: timeout
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
        interval: __priv.countDown * 1000
        onTriggered: __priv.remainingTime -= __priv.countDown
    }

    Image {
        id: picture
        anchors.fill: parent
        source: compositor.settings.background.pictureUrl
        sourceSize.width: width * 0.75
        sourceSize.height: height * 0.75
        fillMode: compositor.settings.convertFillMode(compositor.settings.background.fillMode)
        cache: false
        visible: false
    }

    FastBlur {
        anchors.fill: picture
        source: picture
        radius: 32

        // Catch mouse input
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
        }
    }

    ColumnLayout {
        anchors.centerIn: parent

        Icon {
            id: actionIcon
            size: Units.iconSizes.enormous

            Layout.alignment: Qt.AlignHCenter
        }

        HeadlineLabel {
            id: actionLabel
            horizontalAlignment: Qt.AlignHCenter

            Layout.fillWidth: true
        }

        ProgressBar {
            width: Units.largeSpacing * 5
            from: 0
            to: __priv.timeout
            value: __priv.remainingTime

            Layout.alignment: Qt.AlignHCenter
        }

        Label {
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
                case "suspend":
                    msg = "The system will be suspended automatically in %1 seconds.";
                    break;
                case "hibernate":
                    msg = "The system will be hibernated automatically in %1 seconds.";
                    break;
                default:
                    break;
                }

                return qsTr(msg).arg(__priv.remainingTime);
            }

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            height: Units.largeSpacing * 2
        }

        Row {
            id: buttonsRow
            spacing: Units.smallSpacing
            focus: true
            visible: SessionInterface.canLogOut ||
                     SessionInterface.canPowerOff ||
                     SessionInterface.canRestart ||
                     SessionInterface.canSuspend ||
                     SessionInterface.canHibernate

            IconButton {
                id: logoutButton
                iconName: Qt.resolvedUrl("../images/logout.svg")
                iconSize: Units.iconSizes.medium
                autoExclusive: true
                checkable: true
                checked: __priv.mode == "logout"
                visible: SessionInterface.canLogOut
                onClicked: __priv.mode = "logout"
            }

            IconButton {
                id: poweroffButton
                iconName: "action/power_settings_new"
                iconSize: Units.iconSizes.medium
                autoExclusive: true
                checkable: true
                checked: __priv.mode == "poweroff"
                visible: SessionInterface.canPowerOff
                onClicked: __priv.mode = "poweroff"
            }

            IconButton {
                id: restartButton
                iconName: Qt.resolvedUrl("../images/reload.svg")
                iconSize: Units.iconSizes.medium
                autoExclusive: true
                checkable: true
                checked: __priv.mode == "restart"
                visible: SessionInterface.canRestart
                onClicked: __priv.mode = "restart"
            }

            IconButton {
                id: suspendButton
                iconName: Qt.resolvedUrl("../images/sleep.svg")
                iconSize: Units.iconSizes.medium
                autoExclusive: true
                checkable: true
                checked: __priv.mode == "suspend"
                visible: SessionInterface.canSuspend
                onClicked: __priv.mode = "suspend"
            }

            IconButton {
                id: hibernateButton
                iconName: Qt.resolvedUrl("../images/hibernate.svg")
                iconSize: Units.iconSizes.medium
                autoExclusive: true
                checkable: true
                checked: __priv.mode == "hibernate"
                visible: SessionInterface.canHibernate
                onClicked: __priv.mode = "hibernate"
            }

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            height: Units.largeSpacing * 2
        }

        RowLayout {
            Button {
                text: qsTr("Cancel")
                onClicked: root.cancel()
            }

            Button {
                id: okButton
                highlighted: true
                onClicked: __priv.currentAction()
            }

            Layout.alignment: Qt.AlignHCenter
        }
    }
}
