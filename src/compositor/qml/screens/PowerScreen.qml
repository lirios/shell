/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

import QtQuick 2.4
import QtQuick.Controls.Material 2.0

Item {
    id: powerScreen

    property bool active

    anchors.fill: parent

    signal canceled()

    onActiveChanged: {
        if (!output.primary)
            return

        if (active)
            powerDialog.open()
        else
            powerDialog.close()
    }

    Rectangle {
        anchors.fill: parent

        opacity: powerScreen.active && !output.primary ? 1 : 0
        color: Material.backgroundDimColor

        Behavior on opacity { NumberAnimation { duration: 150 } }
    }

    PowerDialog {
        id: powerDialog

        onCanceled: powerScreen.canceled()
    }
}
