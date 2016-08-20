/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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
import Fluid.Controls 1.0
import "launcher"
import Hawaii.Desktop 1.0
import "../launcher" as Launcher

Indicator {
    id: indicator
    title: qsTr("Applications")
    iconView: AppsIcon {}
    active: popover.visible
    onClicked: popover.open()

    Launcher.LauncherPopOver {
        id: popover
        x: (output.availableGeometry.width - width)/2
        y: (output.availableGeometry.height - height)/2

        parent: screenView

        modal: true

        onAppLaunched: close()
    }
}
