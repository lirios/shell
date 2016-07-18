/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2016 Pier Luigi Fiorini
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Ui 1.0 as FluidUi
import org.hawaiios.launcher 0.1 as CppLauncher

Popup {
    closePolicy: Popup.OnEscape | Popup.OnPressOutside
    modal: true
    focus: true
    implicitWidth: layout.width + (2 * FluidUi.Units.largeSpacing)
    implicitHeight: layout.height + (2 * FluidUi.Units.largeSpacing)

    Material.theme: Material.Dark

    CppLauncher.ProcessRunner {
        id: process
    }

    ColumnLayout {
        id: layout
        anchors.centerIn: parent
        spacing: FluidUi.Units.smallSpacing

        Label {
            text: qsTr("Enter a Command")
        }

        TextField {
            focus: true
            onAccepted: {
                process.launchCommand(text);
                text = "";
                close();
            }

            Layout.minimumWidth: FluidUi.Units.dp(350)
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Close")
            onClicked: close()

            Layout.alignment: Qt.AlignHCenter
        }
    }
}
