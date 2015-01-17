/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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
import org.kde.plasma.core 2.0 as PlasmaCore
import org.hawaii.appchooser 0.1 as AppChooser

RowLayout {
    implicitWidth: (units.iconSizes.large * 3) + (spacing * 3)
    implicitHeight: units.iconSizes.large + (spacing * 2)
    spacing: units.largeSpacing

    AppChooser.SystemModel {
        id: systemModel
    }

    ToolButton {
        width: units.iconSizes.large
        height: width
        iconName: "system-log-out-symbolic"
        tooltip: qsTr("Log out from current session")
        enabled: systemModel.hasCapability(AppChooser.SystemModel.LogoutSession)
        onClicked: compositorRoot.state = "logout"
    }

    ToolButton {
        width: units.iconSizes.large
        height: width
        iconName: "system-shutdown-symbolic"
        tooltip: qsTr("Power off the system")
        enabled: systemModel.hasCapability(AppChooser.SystemModel.Shutdown)
        onClicked: compositorRoot.state = "poweroff"
    }

    ToolButton {
        width: units.iconSizes.large
        height: width
        iconName: "system-reboot-symbolic"
        tooltip: qsTr("Restart the system")
        enabled: systemModel.hasCapability(AppChooser.SystemModel.Reboot)
        onClicked: compositorRoot.state = "restart"
    }
}
