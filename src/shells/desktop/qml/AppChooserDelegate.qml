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

import QtQuick 2.0

MouseArea {
    id: delegateRoot

    property int visualIndex

    property alias icon: content.icon
    property alias label: content.label

    width: grid.cellWidth
    height: grid.cellHeight

    drag.target: theItem

    Item {
        id: theItem

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        width: delegateRoot.width
        height: delegateRoot.height
 
        AppChooserItem {
            id: content
            anchors.fill: parent
        }

        Drag.active: delegateRoot.drag.active
        Drag.source: delegateRoot
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height/ 2

        states: [
            State {
                when: delegateRoot.drag.active

                ParentChange {
                    target: theItem
                    parent: grid
                }
                AnchorChanges {
                    target: theItem
                    anchors.horizontalCenter: undefined
                    anchors.verticalCenter: undefined
                }
            }
        ]
    }

    DropArea {
        anchors {
            fill: parent
            margins: 15
        }
        onEntered: visualModel.items.move(drag.source.visualIndex, delegateRoot.visualIndex)
    }
}
