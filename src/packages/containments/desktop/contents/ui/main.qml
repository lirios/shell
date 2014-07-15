/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Shell 1.0 as Shell

DropArea {
    property Item configButton
    property Item dragOverlay

    id: root
    keys: ["application/x-hawaiishell-element"]
/*
    onEntered: LayoutManager.insertAtCoordinates(dndSpacer, drag.x, drag.y)
    onPositionChanged: LayoutManager.insertAtCoordinates(dndSpacer, drag.x, drag.y)
    onExited: dndSpacer.parent = root
    onDropped: {
        if (drop.proposedAction === Qt.MoveAction || drop.proposedAction === Qt.CopyAction) {
            if (drop.keys === keys)
                drop.acceptProposedAction();
        }
    }
*/

    Flow {
        anchors.fill: parent
        add: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce; duration: 400 }
        }
        populate: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce; duration: 400 }
        }
    }
}
