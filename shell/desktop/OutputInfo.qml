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

Rectangle {
    property int number
    property bool primary

    width: label.paintedWidth + 8
    height: label.paintedHeight + 8
    color: "#80000000"
    radius: 8

    Text {
        id: label
        anchors.centerIn: parent
        color: "white"
        text: number
        font.pointSize: 36
    }

    Rectangle {
        anchors {
            left: parent.left
            top: label.bottom
            right: parent.right
            leftMargin: 4
            rightMargin: 4
        }
        height: 1
        color: "green"
        visible: primary
    }
}
