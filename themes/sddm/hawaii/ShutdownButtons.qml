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

import QtQuick 2.0
import Hawaii.Themes 1.0 as Themes

Row {
    property alias rebootVisible: rebootButton.visible
    property alias powerOffVisible: powerOffButton.visible

    signal rebootRequested()
    signal powerOffRequested()

    id: root

    IconButton {
        id: rebootButton
        iconName: "system-reboot-symbolic"
        width: Themes.Units.iconSizes.medium
        height: width
        color: Themes.Theme.palette.panel.textColor
        visible: false
        onClicked: root.rebootRequested()
    }

    IconButton {
        id: powerOffButton
        iconName: "system-shutdown-symbolic"
        width: Themes.Units.iconSizes.medium
        height: width
        color: Themes.Theme.palette.panel.textColor
        visible: false
        onClicked: root.powerOffRequested()
    }
}
