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
    spacing: Themes.Units.gu(0.5)

    Rectangle {
        height: parent.height
        width: height
        radius: height / 2
        gradient: Gradient {
            GradientStop { color: Themes.Theme.palette.window.closeButtonPrimaryColor; position: 0 }
            GradientStop { color: Themes.Theme.palette.window.closeButtonSecondaryColor; position: 1 }
        }
        border.width: 1
        border.color: Qt.rgba(0, 0, 0, 0.5)

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

    Rectangle {
        height: parent.height
        width: height
        radius: height / 2
        gradient: Gradient {
            GradientStop { color: Themes.Theme.palette.window.buttonPrimaryColor; position: 0 }
            GradientStop { color: Themes.Theme.palette.window.buttonSecondaryColor; position: 1 }
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
            onClicked: root.minimize()
        }
    }

    Rectangle {
        height: parent.height
        width: height
        radius: height / 2
        gradient: Gradient {
            GradientStop { color: Themes.Theme.palette.window.buttonPrimaryColor; position: 0 }
            GradientStop { color: Themes.Theme.palette.window.buttonSecondaryColor; position: 1 }
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
            onClicked: root.maximize()
        }
    }
}
