/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import Fluid.Ui 1.0 as FluidUi

RowLayout {
    implicitWidth: (FluidUi.Units.iconSizes.large * 3) + (spacing * 3)
    implicitHeight: FluidUi.Units.iconSizes.large + (spacing * 2)
    spacing: FluidUi.Units.largeSpacing

    ToolButton {
        width: FluidUi.Units.iconSizes.large
        height: width
        iconName: "system-log-out-symbolic"
        tooltip: qsTr("Log out from current session")
        enabled: SessionInterface.canLogOut
        onClicked: {
            SessionInterface.requestLogOut();
            if (indicator.expanded)
                indicator.triggered(indicator);
        }
    }

    ToolButton {
        width: FluidUi.Units.iconSizes.large
        height: width
        iconName: "system-shutdown-symbolic"
        tooltip: qsTr("Power off the system")
        enabled: SessionInterface.canPowerOff
        onClicked: {
            SessionInterface.requestPowerOff();
            if (indicator.expanded)
                indicator.triggered(indicator);
        }
    }

    ToolButton {
        width: FluidUi.Units.iconSizes.large
        height: width
        iconName: "system-reboot-symbolic"
        tooltip: qsTr("Restart the system")
        enabled: SessionInterface.canRestart
        onClicked: {
            SessionInterface.requestRestart();
            if (indicator.expanded)
                indicator.triggered(indicator);
        }
    }
}
