/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtWayland.Compositor 1.0
import Liri.XWayland 1.0 as LXW
import "windows"

LXW.XWayland {
    id: xwayland

    enabled: liriCompositor.settings.shell.enableXwayland
    manager: LXW.XWaylandManager {
        id: manager
        onShellSurfaceRequested: {
            var shellSurface = shellSurfaceComponent.createObject(manager);
            shellSurface.initialize(manager, window, geometry, overrideRedirect, parentShellSurface);
        }
        onShellSurfaceCreated: {
            var window = xwaylandWindowComponent.createObject(manager, {"shellSurface": shellSurface});
            __private.handleShellSurfaceCreated(window, xwaylandChromeComponent);
        }
    }

    onServerStarted: {
        console.debug("Xwayland server started");

        SessionInterface.setEnvironment("DISPLAY", displayName);
    }

    Component {
        id: xwaylandWindowComponent

        XWaylandWindow {}
    }

    Component {
        id: shellSurfaceComponent

        LXW.XWaylandShellSurface {}
    }

    Component {
        id: xwaylandChromeComponent

        XWaylandChrome {}
    }
}
