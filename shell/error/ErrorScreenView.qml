/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *    Michael Spencer <sonrisesoftware@gmail.com>
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import "../base"

BaseScreenView {
    Material.theme: Material.Dark

    color: Material.color(Material.Red)

    ColumnLayout {
        anchors.centerIn: parent

        spacing: 0

        Icon {
            name: "alert/warning"
            size: 96

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.minimumHeight: 8
        }

        Label {
            text: qsTr("Oh no!")
            font: FluidStyle.display1Font
            color: Material.primaryTextColor

            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: qsTr("Something went wrong and the desktop failed to load.")
            font: FluidStyle.subheadingFont
            color: Material.primaryTextColor

            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.minimumHeight: 24
        }

        Button {
            text: qsTr("Quit")
            onClicked: Qt.quit()

            Layout.alignment: Qt.AlignHCenter

            Material.background: "white"
            Material.theme: Material.Light
            Material.elevation: 2
        }
    }
}
