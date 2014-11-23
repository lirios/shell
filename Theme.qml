/****************************************************************************
 * This file is part of Kahai.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

pragma Singleton

import QtQuick 2.0

QtObject {
    property var window: QtObject {
        readonly property color textColor: "#ffffff"
        readonly property color textEffectColor: "#000000"
        readonly property color backgroundColor: "#3e4243"
        readonly property color primaryColor: "#494c4d"
        readonly property color secondaryColor: "#6f7678"
    }

    property var view: QtObject {
        readonly property color textColor: "#000000"
        readonly property color textEffectColor: "#ffffff"
        readonly property color backgroundColor: "#f1f1f1"
        readonly property color selectedTextColor: "#ffffff"
        readonly property color selectedTextEffectColor: "#000000"
        readonly property color selectedBackgroundColor: "#5cb7d7"
        readonly property color primaryColor: "#cdcdcd"
        readonly property color secondaryColor: "#ececec"
    }

    property var panel: QtObject {
        readonly property color textColor: "#ffffff"
        readonly property color textEffectColor: "#000000"
        readonly property color backgroundColor: "#31363b"
        readonly property color selectedTextColor: "#ffffff"
        readonly property color selectedTextEffectColor: "#000000"
        readonly property color selectedBackgroundColor: "#5cb7d7"
        readonly property color primaryColor: "#464e57"
        readonly property color secondaryColor: "#67717a"
    }

    function rgba(color, a) {
        var newColor = color;
        newColor.a = a;
        return newColor;
    }
}
