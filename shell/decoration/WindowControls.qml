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

import QtQuick 2.0
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

Row {
    signal close()
    signal minimize()
    signal maximize()

    id: root
    spacing: Themes.Units.dp(5)

    Rectangle {
        width: height
        height: parent.height
        radius: height / 2
        gradient: Gradient {
            GradientStop { color: "#e12424"; position: 0 }
            GradientStop { color: "#ab0000"; position: 1 }
        }
        border.width: 1
        border.color: "#7f800000"

        Components.Icon {
            anchors {
                fill: parent
                margins: Themes.Units.gu(0.25)
            }
            iconName: "window-close-symbolic"
            color: Themes.Theme.palette.window.textColor
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.close()
        }
    }

    Item {
        width: Themes.Units.dp(2)
        height: parent.height
    }

    Rectangle {
        width: height
        height: parent.height
        radius: height / 2
        gradient: Gradient {
            GradientStop { color: Qt.lighter(Themes.Theme.palette.window.primaryColor, 1.3); position: 0 }
            GradientStop { color: Qt.darker(Themes.Theme.palette.window.primaryColor, 1.3); position: 1 }
        }
        border.width: 1
        border.color: Qt.rgba(0, 0, 0, 0.5)

        Components.Icon {
            anchors {
                fill: parent
                margins: Themes.Units.gu(0.25)
            }
            iconName: "window-maximize-symbolic"
            color: Themes.Theme.palette.window.textColor
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.maximize()
        }
    }

    Rectangle {
        width: height
        height: parent.height
        radius: height / 2
        gradient: Gradient {
            GradientStop { color: Qt.lighter(Themes.Theme.palette.window.primaryColor, 1.3); position: 0 }
            GradientStop { color: Qt.darker(Themes.Theme.palette.window.primaryColor, 1.3); position: 1 }
        }
        border.width: 1
        border.color: Qt.rgba(0, 0, 0, 0.5)

        Components.Icon {
            anchors {
                fill: parent
                margins: Themes.Units.gu(0.25)
            }
            iconName: "window-minimize-symbolic"
            color: Themes.Theme.palette.window.textColor
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.minimize()
        }
    }
}
