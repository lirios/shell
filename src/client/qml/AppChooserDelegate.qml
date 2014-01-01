/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Fluid.Ui 1.0 as FluidUi

MouseArea {
    id: delegateRoot

    property alias icon: iconItem.iconSource
    property alias label: labelItem.text

    width: grid.cellWidth
    height: grid.cellHeight

    ColumnLayout {
        anchors.fill: parent

        FluidUi.Icon {
            id: iconItem
            width: 64
            height: 64

            Layout.alignment: Qt.AlignCenter
        }

        Label {
            id: labelItem
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignTop
            wrapMode: Text.Wrap
            elide: Text.ElideRight

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
