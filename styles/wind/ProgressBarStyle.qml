/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import QtQuick.Controls.Styles 1.1 as QtControlsStyle
import QtQuick.Controls.Styles.Wind 1.0 as Wind
import org.kde.plasma.core 2.0 as PlasmaCore
import "private/Utils.js" as Utils

QtControlsStyle.ProgressBarStyle {
    id: style
    background: Rectangle {
        property color baseColor: Wind.Palette.panel.backgroundColor
        property color highlightColor: Wind.Palette.panel.selectedBackgroundColor

        implicitWidth: units.gridUnit * 20
        implicitHeight: units.gridUnit
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
        property color progressColor: Wind.Palette.panel.selectedBackgroundColor

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
