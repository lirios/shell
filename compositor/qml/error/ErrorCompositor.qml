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

import QtQuick 2.5
import QtQuick.Window 2.0
import QtWayland.Compositor 1.0
import Liri.WaylandServer 1.0
import Liri.Shell 1.0

WaylandCompositor {
    id: compositor

    onCreatedChanged: {
        if (compositor.created)
            shellHelper.start(compositor.socketName);
    }

    onSurfaceRequested: {
        var surface = surfaceComponent.createObject(compositor, {});
        surface.initialize(compositor, client, id, version);
    }

    QtObject {
        id: __private

        property variant outputs: []
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Alt+Backspace"
        onActivated: Qt.quit()
    }

    ScreenManager {
        id: screenManager

        onScreenAdded: {
            var view = outputComponent.createObject(compositor, {
                "compositor": compositor,
                "nativeScreen": screen
            });

            __private.outputs.push(view);
        }
        onScreenRemoved: {
            var index = screenManager.indexOf(screen);

            if (index < __private.outputs.length) {
                var output = __private.outputs[index];
                __private.outputs.splice(index, 1);
                output.destroy();
            }
        }
        onPrimaryScreenChanged: {
            var index = screenManager.indexOf(screen);

            if (index < __private.outputs.length)
                compositor.defaultOutput = __private.outputs[index];
        }
    }

    // Output component
    Component {
        id: outputComponent

        ExtendedOutput {
            readonly property bool primary: compositor.defaultOutput === this
            readonly property alias grabItem: grabItem

            id: output
            manufacturer: nativeScreen.manufacturer
            model: nativeScreen.model
            position: nativeScreen.position
            physicalSize: nativeScreen.physicalSize
            subpixel: nativeScreen.subpixel
            transform: nativeScreen.transform
            scaleFactor: nativeScreen.scaleFactor
            sizeFollowsWindow: true

            window: Window {
                x: nativeScreen.position.x
                y: nativeScreen.position.y
                width: nativeScreen.size.width
                height: nativeScreen.size.height
                flags: Qt.FramelessWindowHint

                // Grab surface from shell helper
                WaylandQuickItem {
                    id: grabItem
                    focusOnClick: false
                    onSurfaceChanged: {
                        shellHelper.grabCursor(ShellHelper.ArrowGrabCursor);
                        if (output.primary)
                            grabItem.setPrimary();
                    }
                }

                WaylandMouseTracker {
                    id: mouseTracker
                    anchors.fill: parent
                    windowSystemCursorEnabled: platformName !== "liri"

                    ErrorScreenView {
                        id: screenView
                        anchors.fill: parent
                    }

                    WaylandCursorItem {
                        id: cursor
                        seat: output.compositor.defaultSeat
                        x: mouseTracker.mouseX - hotspotX
                        y: mouseTracker.mouseY - hotspotY
                        visible: mouseTracker.containsMouse && !mouseTracker.windowSystemCursorEnabled
                    }
                }
            }
        }
    }

    // Surface component
    Component {
        id: surfaceComponent

        WaylandSurface {}
    }

    // Shell helper
    ShellHelper {
        id: shellHelper
        onGrabSurfaceAdded: {
            for (var i = 0; i < __private.outputs.length; i++)
                __private.outputs[i].grabItem.surface = surface;
        }
    }
}
