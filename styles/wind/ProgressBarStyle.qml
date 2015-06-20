/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import QtQuick.Controls.Styles 1.1 as QtControlsStyle
import Hawaii.Themes 1.0 as Themes

QtControlsStyle.ProgressBarStyle {
    id: style
    background: Rectangle {
        property color baseColor: Themes.Theme.palette.panel.backgroundColor
        property color highlightColor: Themes.Theme.palette.panel.selectedBackgroundColor

        implicitWidth: Themes.Units.gu(20)
        implicitHeight: Themes.Units.gu(1)
        border.color: Qt.darker(baseColor, 3.0)
        radius: 6
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.darker(baseColor, 2.0); }
            GradientStop { position: 0.38; color: Qt.darker(baseColor, 1.5); }
            GradientStop { position: 0.99; color: Qt.darker(baseColor, 1.2); }
            GradientStop { position: 1.0; color: Qt.darker(baseColor, 1.1); }
        }
        antialiasing: true
        opacity: 0.8

        Rectangle {
            width: parent.width * control.value / control.maximumValue
            height: parent.height
            border.color: Qt.darker(highlightColor, 3.0)
            radius: 6
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#2a536e"; }
                GradientStop { position: 0.38; color: "#3d7aa0"; }
                GradientStop { position: 0.99; color: "#468bb7"; }
                GradientStop { position: 1.0; color: "#4991bf"; }
            }
            antialiasing: true
        }
    }
    progress: Rectangle {
        property color progressColor: Themes.Theme.palette.panel.selectedBackgroundColor

        anchors {
            fill: parent
            margins: 0
        }
        radius: 6
        antialiasing: true
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.lighter(progressColor, 1.3) }
            GradientStop { position: 1.4; color: progressColor }
        }
        border.color: Qt.darker(progressColor, 1.2)

        Image {
            width: parent.width + 25
            source: "images/progress-indeterminate.png"
            fillMode: Image.Tile
            visible: control.indeterminate
            clip: true

            NumberAnimation on x {
                from: -39
                to: 0
                running: control.indeterminate
                duration: 1200
                loops: Animation.Infinite
            }
        }
    }
}
