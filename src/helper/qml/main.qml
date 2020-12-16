/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQml 2.1
import QtQuick 2.0
import QtQuick.Window 2.9
import Liri.WaylandClient 1.0 as WaylandClient
import "components" as Components

Item {
    property int refCount: 0

    onRefCountChanged: {
        if (shell.active && refCount == 0)
            shell.sendReady();
    }

    /*
     * Key bindings
     */

    KeyBindings {}

    /*
     * Shell
     */

    WaylandClient.LiriShell {
        id: shell

        onActiveChanged: {
            if (active) {
                registerGrabSurface(grabWindow);
                grabWindow.visible = true;
            }
        }
    }

    WaylandClient.WlrLayerShellV1 {
        id: layerShell
    }

    Osd {}

    Components.BorderlessWindow {
        id: grabWindow

        width: 1
        height: 1
    }

    BackgroundWindow {}
}
