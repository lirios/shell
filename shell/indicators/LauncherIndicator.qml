/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import GreenIsland 1.0 as GreenIsland
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import ".."
import "../launcher" as Launcher

Indicator {
    id: indicator
    name: "launcher"
    text: qsTr("Menu")
    onTriggered: __priv.shown = !__priv.shown

    QtObject {
        id: __priv

        property bool shown: false

        onShownChanged: {
            if (shown)
                popover.show();
            else
                popover.hide();
        }
    }

    Components.Loadable {
        id: popover
        component: Launcher.LauncherPopOver {
            parent: panel.parent
            origin: panel.mapToItem(panel.parent, 0, 0)
            location: Qt.AlignBottom
            onAppLaunched: __priv.shown = false
            onDismissed: __priv.shown = false
        }
    }

    Connections {
        target: GreenIsland.KeyBindings
        onKeyBindingReleased: if (name == "mainMenu") __priv.shown = !__priv.shown
    }
}

