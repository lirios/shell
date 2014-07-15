/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtGraphicalEffects 1.0
import Fluid.Ui 1.0
import Hawaii.Shell.Styles 1.0
import Hawaii.Shell.Styles.Base 1.0

PopupStyle {
    property int shadowSize: 8

    padding {
        left: shadowSize
        top: shadowSize
        right: shadowSize
        bottom: shadowSize
    }

    panel: Item {
        Rectangle {
            id: border
            anchors {
                fill: parent
                margins: shadowSize
            }
            border.color: "#999"
            radius: 6
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#f4f4f4" }
                GradientStop { position: 1.0; color: "#dcdcdc" }
            }
            visible: false

            NoiseBackground {
                anchors {
                    fill: parent
                    margins: 3
                }
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#f4f4f4" }
                    GradientStop { position: 1.0; color: "#dcdcdc" }
                }
            }
        }

        DropShadow {
            anchors.fill: border
            source: border
            radius: 8
            samples: 16
            fast: true
            spread: 0
            color: Qt.rgba(0, 0, 0, 0.5)
            transparentBorder: true
        }
    }
}
