/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import Hawaii.Themes 1.0 as Themes

TextField {
    property color textColor: "#000000"
    property color backgroundColor: "#ffffff"
    property color selectionColor: Themes.Theme.palette.panel.selectedBackgroundColor
    property color selectedTextColor: Themes.Theme.palette.panel.selectedTextColor
    property color focusColor: Themes.Theme.palette.panel.selectedBackgroundColor

    id: passwordField
    placeholderText: qsTr("Password");
    echoMode: TextInput.Password
    style: TextFieldStyle {
        textColor: passwordField.textColor
        placeholderTextColor: Qt.rgba(passwordField.textColor.r, passwordField.textColor.g, passwordField.textColor.b, 0.5)
        selectionColor: passwordField.selectionColor
        selectedTextColor: passwordField.selectedTextColor
        font.family: "Noto Sans" // Themes.Theme.defaultFont.family
        font.pointSize: 12
        font.bold: true
        background: Item {
            implicitWidth: passwordField.width
            implicitHeight: Themes.Theme.mSize(Themes.Theme.defaultFont).height + Themes.Units.gu(0.5)

            Rectangle {
                anchors.fill: parent
                anchors.bottomMargin: -1
                color: Qt.rgba(passwordField.textColor.r, passwordField.textColor.g, passwordField.textColor.b, 0.267)
                radius: baserect.radius
            }

            Rectangle {
                id: baserect
                anchors.fill: parent
                radius: Themes.Units.dp(4)
                gradient: Gradient {
                    GradientStop { position: 0.0; color: Qt.lighter(passwordField.backgroundColor, 0.8) }
                    GradientStop { position: 0.3; color: passwordField.backgroundColor }
                    GradientStop { position: 1.0; color: passwordField.backgroundColor }
                }
                border.color: control.activeFocus ? passwordField.focusColor : Qt.darker(passwordField.backgroundColor, 1.5)
            }
        }
    }
}
