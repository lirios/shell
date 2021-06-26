// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtWayland.Compositor 1.15
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
