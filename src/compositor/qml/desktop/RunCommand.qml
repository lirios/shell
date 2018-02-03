/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import Liri.Launcher 0.1 as CppLauncher
import Fluid.Controls 1.0 as FluidControls

Dialog {
    modal: true
    focus: true

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    CppLauncher.ProcessRunner {
        id: process
    }

    standardButtons: Dialog.Cancel

    ColumnLayout {
        width: parent.width
        spacing: FluidControls.Units.smallSpacing

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

            Layout.minimumWidth: 350
            Layout.fillWidth: true
        }
    }
}
