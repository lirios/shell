/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import Fluid.Controls 1.0

RowLayout {
    implicitWidth: (Units.iconSizes.large * 3) + (spacing * 3)
    implicitHeight: Units.iconSizes.large + (spacing * 2)
    spacing: Units.smallSpacing

    IconButton {
        iconSize: Units.iconSizes.smallMedium
        iconName: Qt.resolvedUrl("../images/logout.svg")
        enabled: SessionInterface.canLogOut
        onClicked: {
            launcherPopOver.close();
            SessionInterface.requestLogOut();
        }

        ToolTip.text: qsTr("Log out")
    }

    IconButton {
        iconSize: Units.iconSizes.smallMedium
        iconName: "action/power_settings_new"
        enabled: SessionInterface.canPowerOff
        onClicked: {
            launcherPopOver.close();
            SessionInterface.requestPowerOff();
        }

        ToolTip.text: qsTr("Power off")
    }
}
