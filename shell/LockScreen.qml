/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.4
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtGraphicalEffects 1.0
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaii.session 1.0 as Session
import "components" as CustomComponents
import "."

Components.Showable {
    id: root
    showAnimation: YAnimator {
        target: root
        easing.type: Easing.OutQuad
        duration: Themes.Units.longDuration
        from: -root.height
        to: 0
    }
    hideAnimation: YAnimator {
        target: root
        easing.type: Easing.OutQuad
        duration: Themes.Units.longDuration
        from: 0
        to: -root.height
    }
    visible: true

    QtObject {
        id: __priv

        property string timeFormat
    }

    Image {
        id: picture
        anchors.fill: parent
        source: ShellSettings.lockScreen.pictureUrl
        sourceSize.width: width * 0.75
        sourceSize.height: height * 0.75
        fillMode: ShellSettings.convertFillMode(ShellSettings.lockScreen.fillMode)
        cache: false
        visible: false
    }

    FastBlur {
        anchors.fill: picture
        source: picture
        radius: 32
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
            color: Themes.Theme.palette.window.textColor
            style: Text.Raised
            styleColor: Themes.Theme.palette.window.textEffectColor

            Layout.alignment: Qt.AlignCenter
        }

        Label {
            id: dateLabel
            font.pointSize: 36
            color: Themes.Theme.palette.window.textColor
            style: Text.Raised
            styleColor: Themes.Theme.palette.window.textEffectColor

            Layout.alignment: Qt.AlignCenter
        }

        Item {
            Layout.fillHeight: true
        }

        Rectangle {
            color: "#80000000"

            Column {
                anchors.centerIn: parent
                spacing: Themes.Units.smallSpacing

                CustomComponents.TextField {
                    id: passwordField
                    placeholderText: qsTr("Password")
                    width: Themes.Units.gu(20)
                    focus: true
                    echoMode: TextInput.Password
                    onAccepted: {
                        Session.SessionInterface.unlockSession(text, function(succeded) {
                            if (!succeded) {
                                text = "";
                                errorLabel.text = qsTr("Sorry, wrong password. Please try again.");
                                errorTimer.start();
                            }
                        });
                    }
                }

                Text {
                    id: errorLabel
                    color: "red"
                    text: " "
                    height: paintedHeight
                    renderType: Text.NativeRendering

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
            Layout.preferredHeight: Themes.Units.gu(5)
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
