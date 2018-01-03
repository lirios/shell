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

import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Liri.Shell 1.0 as Shell
import Fluid.Controls 1.0 as FluidControls

Shell.Indicator {
    title: qsTr("Date & Time")
    text: Qt.formatTime(dateTime.now)
    tooltip: Qt.formatDate(dateTime.now, Locale.LongFormat)

    component: Item {
        FluidControls.DatePicker {
            id: datePicker

            Material.theme: Material.Light

            anchors.horizontalCenter: parent.horizontalCenter
            orientation: FluidControls.DatePicker.Portrait
            footer: DialogButtonBox {
                Material.background: "transparent"

                padding: 0

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Today")
                    flat: true
                    onClicked: datePicker.selectedDate = new Date()
                }
            }
        }
    }
}
