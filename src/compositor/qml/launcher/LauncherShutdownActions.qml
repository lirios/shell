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

import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3
import Fluid.Controls 1.0 as FluidControls
import Liri.Device 1.0 as LiriDevice

RowLayout {
    implicitWidth: (FluidControls.Units.iconSizes.large * 3) + (spacing * 3)
    implicitHeight: FluidControls.Units.iconSizes.large + (spacing * 2)
    spacing: FluidControls.Units.smallSpacing

    ToolButton {
        icon.source: Qt.resolvedUrl("../images/logout.svg")
        onClicked: {
            launcherPopOver.close();
            liriCompositor.defaultOutput.screenView.showLogout();
        }

        ToolTip.text: qsTr("Log out")
        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
        ToolTip.visible: hovered
    }

    ToolButton {
        icon.source: FluidControls.Utils.iconUrl("action/lock")
        onClicked: {
            launcherPopOver.close();
            SessionInterface.lock();
        }

        ToolTip.text: qsTr("Lock Session")
        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
        ToolTip.visible: hovered
    }

    ToolButton {
        icon.source: FluidControls.Utils.iconUrl("action/power_settings_new")
        enabled: LiriDevice.LocalDevice.canPowerOff
        onClicked: {
            launcherPopOver.close();
            liriCompositor.defaultOutput.screenView.showPowerOff();
        }

        ToolTip.text: qsTr("Power off")
        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
        ToolTip.visible: hovered
    }
}
