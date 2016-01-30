/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.4
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import Hawaii.Components 1.0 as Components
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes

Components.Showable {
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
        duration: Themes.Units.longDuration
        from: 0.0
        to: 1.0
    }
    hideAnimation: OpacityAnimator {
        target: root
        easing.type: Easing.InSine
        duration: Themes.Units.longDuration
        from: 1.0
        to: 0.0
    }
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
        },
        State {
            name: "suspend"
            PropertyChanges { target: __priv; currentAction: suspendRequested }
            PropertyChanges { target: actionIcon; iconName: "system-suspend-symbolic" }
            PropertyChanges { target: actionLabel; text: qsTr("Suspend") }
            PropertyChanges { target: okButton; text: qsTr("Suspend") }
        },
        State {
            name: "hibernate"
            PropertyChanges { target: __priv; currentAction: hibernateRequested }
            PropertyChanges { target: actionIcon; iconName: "system-suspend-hibernate-symbolic" }
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

    QtObject {
        id: __priv

        property string mode: "poweroff"
        property real timeout: 30
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
        source: hawaiiCompositor.settings.background.pictureUrl
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

        // Catch mouse input
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
        }
    }

    ColumnLayout {
        anchors.centerIn: parent

        Components.Icon {
            id: actionIcon
            width: Themes.Units.iconSizes.enormous
            height: width
            color: Themes.Theme.palette.panel.textColor
            cache: false

            Layout.alignment: Qt.AlignHCenter
        }

        Controls.Heading {
            id: actionLabel
            level: 2
            color: Themes.Theme.palette.panel.textColor

            Layout.alignment: Qt.AlignHCenter
        }

        ProgressBar {
            width: Themes.Units.largeSpacing * 5
            minimumValue: 0
            maximumValue: __priv.timeout
            value: __priv.remainingTime

            Layout.alignment: Qt.AlignHCenter
        }

        Text {
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
            renderType: Text.NativeRendering
            color: Themes.Theme.palette.panel.textColor

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            height: Themes.Units.largeSpacing * 2
        }

        Row {
            spacing: Themes.Units.smallSpacing
            focus: true
            visible: SessionInterface.canLogOut ||
                     SessionInterface.canPowerOff ||
                     SessionInterface.canRestart ||
                     SessionInterface.canSuspend ||
                     SessionInterface.canHibernate

            ExclusiveGroup { id: group }

            ToolButton {
                id: logoutButton
                exclusiveGroup: group
                iconName: "system-log-out-symbolic"
                width: Themes.Units.iconSizes.medium
                height: width
                checkable: true
                checked: __priv.mode == "logout"
                visible: SessionInterface.canLogOut
                onClicked: __priv.mode = "logout"
            }

            ToolButton {
                id: poweroffButton
                exclusiveGroup: group
                iconName: "system-shutdown-symbolic"
                width: Themes.Units.iconSizes.medium
                height: width
                checkable: true
                checked: __priv.mode == "poweroff"
                visible: SessionInterface.canPowerOff
                onClicked: __priv.mode = "poweroff"
            }

            ToolButton {
                id: restartButton
                exclusiveGroup: group
                iconName: "system-reboot-symbolic"
                width: Themes.Units.iconSizes.medium
                height: width
                checkable: true
                checked: __priv.mode == "restart"
                visible: SessionInterface.canRestart
                onClicked: __priv.mode = "restart"
            }

            ToolButton {
                id: suspendButton
                exclusiveGroup: group
                iconName: "system-suspend-symbolic"
                width: Themes.Units.iconSizes.medium
                height: width
                checkable: true
                checked: __priv.mode == "suspend"
                visible: SessionInterface.canSuspend
                onClicked: __priv.mode = "suspend"
            }

            ToolButton {
                id: hibernateButton
                exclusiveGroup: group
                iconName: "system-suspend-hibernate-symbolic"
                width: Themes.Units.iconSizes.medium
                height: width
                checkable: true
                checked: __priv.mode == "hibernate"
                visible: SessionInterface.canHibernate
                onClicked: __priv.mode = "hibernate"
            }

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            height: Themes.Units.largeSpacing * 2
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
}
