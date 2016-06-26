/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Ui 1.0 as FluidUi
import Fluid.Effects 1.0 as FluidEffects

FluidUi.Showable {
    id: root
    showAnimation: YAnimator {
        target: root
        easing.type: Easing.OutQuad
        duration: FluidUi.Units.longDuration
        from: -root.height
        to: 0
    }
    hideAnimation: YAnimator {
        target: root
        easing.type: Easing.OutQuad
        duration: FluidUi.Units.longDuration
        from: 0
        to: -root.height
    }
    visible: true
    onVisibleChanged: {
        // Activate password field
        if (visible)
            passwordField.forceActiveFocus();
    }

    Material.theme: Material.Dark

    QtObject {
        id: __priv

        property string timeFormat
    }

    Image {
        id: picture
        anchors.fill: parent
        source: hawaiiCompositor.settings.lockScreen.pictureUrl
        sourceSize.width: width * 0.75
        sourceSize.height: height * 0.75
        fillMode: hawaiiCompositor.settings.convertFillMode(hawaiiCompositor.settings.lockScreen.fillMode)
        cache: false
        visible: false
    }

    FastBlur {
        id: blur
        anchors.fill: picture
        source: picture
        radius: 32
    }

    FluidEffects.Vignette {
        anchors.fill: parent
        source: blur
        radius: 8
        brightness: 0.2
    }

    Timer {
        id: timer
        running: false
        triggeredOnStart: true
        interval: 30000
        onTriggered: {
            var now = new Date();
            timeLabel.text = Qt.formatTime(now, __priv.timeFormat);
            dateLabel.text = Qt.formatDate(now, Locale.LongFormat);
        }
    }

    ColumnLayout {
        anchors.fill: parent

        Item {
            Layout.fillHeight: true
        }

        Label {
            id: timeLabel
            font.pointSize: 42
            style: Text.Raised
            styleColor: Material.backgroundTextColor

            Layout.alignment: Qt.AlignCenter
        }

        Label {
            id: dateLabel
            font.pointSize: 36
            style: Text.Raised
            styleColor: Material.backgroundTextColor

            Layout.alignment: Qt.AlignCenter
        }

        Item {
            Layout.preferredHeight: FluidUi.Units.dp(100)
        }

        Pane {
            Column {
                anchors.centerIn: parent
                spacing: FluidUi.Units.smallSpacing

                TextField {
                    id: passwordField
                    placeholderText: qsTr("Password")
                    width: FluidUi.Units.gu(20)
                    focus: true
                    echoMode: TextInput.Password
                    onAccepted: {
                        SessionInterface.unlockSession(text, function(succeded) {
                            if (!succeded) {
                                text = "";
                                errorLabel.text = qsTr("Sorry, wrong password. Please try again.");
                                errorTimer.start();
                            }
                        });
                    }
                }

                Label {
                    id: errorLabel
                    color: "red"
                    text: " "
                    height: paintedHeight

                    Timer {
                        id: errorTimer
                        interval: 5000
                        onTriggered: {
                            errorLabel.text = " ";
                            running = false;
                        }
                    }
                }
            }

            Layout.fillWidth: true
            Layout.preferredHeight: FluidUi.Units.gu(5)
        }

        Item {
            Layout.fillHeight: true
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        onClicked: passwordField.forceActiveFocus()
    }

    Component.onCompleted: {
        // Remove seconds from time format
        __priv.timeFormat = Qt.locale().timeFormat(Locale.ShortFormat).replace(/.ss?/i, "");

        // Start timer
        timer.start();

        // Activate password field
        passwordField.forceActiveFocus();
    }
}
