/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

    enabled: true
    onShellSurfaceRequested: {
        var shellSurface = shellSurfaceComponent.createObject(manager);
        shellSurface.initialize(manager, window, geometry, overrideRedirect);
    }
    onShellSurfaceCreated: __private.handleShellSurfaceCreated(shellSurface, xchromeComponent)

    Component {
        id: shellSurfaceComponent

        LXW.XWaylandShellSurface {
            id: shellSurface

            property bool activated: true
            property bool minimized: false
            property string canonicalAppId: applicationManager.canonicalizeAppId(appId)
            property string iconName: "unknown"

            readonly property bool decorated: windowType != Qt.Popup
            readonly property bool hasDropShadow: !maximized && !fullscreen

            property WaylandSurface parentWlSurface: null
            property point offset: Qt.point(0, 0)

            signal destruction()

            QtObject {
                id: details

                property bool registered: false
                property bool responsive: true
            }

            Connections {
                target: defaultSeat
                onKeyboardFocusChanged: shellSurface.activated = newFocus == surface
            }

            onActivatedChanged: {
                if (details.registered && activated)
                    applicationManager.focusShellSurface(shellSurface);
            }
            onCanonicalAppIdChanged: {
                if (!details.registered && canonicalAppId) {
                    // Register application
                    applicationManager.registerShellSurface(shellSurface);
                    details.registered = true;

                    // Focus icon in the panel
                    if (activated)
                        applicationManager.focusShellSurface(shellSurface);
                }

                // Set icon name when appId changes
                var appIconName = applicationManager.getIconName(canonicalAppId);
                iconName = appIconName ? appIconName : "unknown";
            }
            onWindowTypeChanged: {
                switch (windowType) {
                case Qt.Popup:
                    offset.x = x;
                    offset.y = y;
                    break;
                default:
                    offset.x = 0;
                    offset.y = 0;
                    break;
                }
            }

            function requestSize(w, h) {
                shellSurface.sendResize(Qt.size(w, h));
            }

            Component.onDestruction: {
                shellSurface.destruction();
                __private.handleShellSurfaceDestroyed(shellSurface);
            }
        }
    }

    Component {
        id: xchromeComponent

        XWaylandChrome {
            inputEventsEnabled: !output.screenView.locked
        }
    }
}
