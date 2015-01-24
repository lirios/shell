/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

Item {
    property Item visualParent
    property Item visualLayer
    property Item popup
    readonly property bool showing: popup ? popup.opacity > 0.0 : false

    signal closed()

    id: root
    width: popup ? popup.width : 0
    height: popup ? popup.height : 0

    function open() {
        if (showing)
            return;

        var x = (visualParent.width * 0.5) - (popup.width * 0.5);
        var y = -popup.height;
        openAt(x, y);
    }

    function openAt(x, y) {
        if (showing)
            return;

        var pt = compositorRoot.mapFromItem(visualParent, x, y);
        popup.parent = visualLayer;
        popup.x = pt.x;
        popup.y = pt.y;
        popup.opacity = 1.0;
    }

    function close() {
        if (!showing)
            return;

        popup.opacity = 0.0;
    }
}
