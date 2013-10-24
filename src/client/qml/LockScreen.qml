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
import QtGraphicalEffects 1.0
import FluidUi 0.2 as FluidUi
import Hawaii.Shell 1.0

Item {
    property variant sessionManager: Shell.service("SessionManager")

    Timer {
        interval: 1000
        running: true
        repeat: true
        triggeredOnStart: true
        onTriggered: {
            dateText.text = Qt.formatDate(new Date(), Qt.locale().dateFormat(Locale.LongFormat));
            timeText.text = Qt.formatTime(new Date(), Qt.locale().timeFormat(Locale.ShortFormat));
        }
    }

    Background {
        id: background
        anchors.fill: parent
        visible: false
    }

    FastBlur {
        id: fastBlurEffect
        anchors.fill: parent
        source: background
        radius: 32
        visible: false
    }

    EffectVignette {
        anchors.fill: fastBlurEffect
        source: fastBlurEffect
        radius: 8
    }

    ColumnLayout {
        anchors.centerIn: parent

        Text {
            id: timeText
            font.family: "Raleway [Medium]"
            font.pointSize: 52
            color: "white"
            style: Text.Raised
            styleColor: Qt.rgba(0, 0, 0, 0.7)

            Layout.alignment: Qt.AlignCenter
        }

        Text {
            id: dateText
            font.family: "Raleway [Thin]"
            font.pointSize: 36
            color: "white"
            style: Text.Raised
            styleColor: Qt.rgba(0, 0, 0, 0.7)

            Layout.alignment: Qt.AlignCenter
        }

        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: 250
        }

        MouseArea {
            width: 96
            height: 96
            onReleased: sessionManager.unlock()

            Rectangle {
                anchors.fill: parent
                radius: height
                color: "#80222222"
                border.color: "#222"
                antialiasing: true

                FluidUi.Icon {
                    anchors.fill: parent
                    anchors.margins: 20
                    iconName: "changes-allow-symbolic"
                    color: "white"
                }
            }

            Layout.alignment: Qt.AlignCenter
        }
    }
}
