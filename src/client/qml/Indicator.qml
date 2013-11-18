/****************************************************************************
 * This file is part of Hawaii Shell.
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
import QtQuick.Layouts 1.0
import FluidUi 0.2 as FluidUi

Item {
    height: 32

    property alias iconName: icon.iconName
    default property alias content: container.data

    RowLayout {
        anchors {
            fill: parent
            margins: 10
        }

        FluidUi.Icon {
            id: icon
            width: 22
            height: 22
            color: "black"

            Layout.alignment: Qt.AlignVCenter
        }

        Item {
            id: container

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Layout.fillWidth: true
}
