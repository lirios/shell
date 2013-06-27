/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Window 2.0
import QtQuick.Controls 1.0
import FluidCore 1.0

Window {
    color: "transparent"

    // Padding
    // TODO: Take it from parameters
    property real padding: 2

    // Panel height
    property int realSize: 24 + (padding * 2)
    property int size: realSize

    Rectangle {
        id: contents
        anchors.fill: parent
        color: "#f1f1f1"

        PanelView {
            anchors {
                left: parent.left
                top: parent.top
            }
            width: parent.width
            height: parent.height - 1
        }

        Rectangle {
            id: border
            color: "#999"
            height: 1
        }
    }
}
