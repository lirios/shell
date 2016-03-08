/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Fluid.Ui 1.0 as FluidUi
import "../components" as CustomComponents

CustomComponents.PopupMenu {
    property var window

    id: menu
    width: FluidUi.Units.dp(200)
    height: column.height

    Column {
        id: column
        width: parent.width

        CustomComponents.MenuItem {
            text: qsTr("Minimize")
        }

        CustomComponents.MenuItem {
            text: qsTr("Restore")
        }

        CustomComponents.MenuItem {
            text: qsTr("Maximize")
        }

        CustomComponents.MenuSeparator {}

        CustomComponents.MenuItem {
            text: qsTr("Move")
        }

        CustomComponents.MenuItem {
            text: qsTr("Resize")
        }

        CustomComponents.MenuSeparator {}

        CustomComponents.MenuItem {
            text: qsTr("Close")
        }
    }
}
