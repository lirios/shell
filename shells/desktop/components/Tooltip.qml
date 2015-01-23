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
import "private" as Private

Item {
    property string text
    property Item visualParent: parent
    readonly property bool showing: tooltipLoader.loaded ? tooltipLoader.item.showing : false

    signal closed()

    id: root
    width: tooltipLoader.loaded ? tooltipLoader.item.width : 0
    height: tooltipLoader.loaded ? tooltipLoader.item.height : 0

    Component {
        id: tooltipComponent

        Private.BasicTooltip {
            id: tooltip
            text: root.text
            visualParent: root.visualParent
            visualLayer: notificationsLayer
            onClosed: {
                tooltipLoader.sourceComponent = undefined;
                root.closed();
            }
        }
    }

    Loader {
        property bool loaded: status == Loader.Ready

        id: tooltipLoader
    }

    Timer {
        running: root.showing
        repeat: false
        interval: 1500
        onTriggered: root.close()
    }

    function open() {
        if (!tooltipLoader.loaded)
            tooltipLoader.sourceComponent = tooltipComponent;
        tooltipLoader.item.open();
    }

    function openAt(x, y) {
        if (!tooltipLoader.loaded)
            tooltipLoader.sourceComponent = tooltipComponent;
        tooltipLoader.item.openAt(x, y);
    }

    function close() {
        if (tooltipLoader.loaded)
            tooltipLoader.item.close();
    }
}
