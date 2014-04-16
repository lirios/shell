/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

var windowList = null;

function isShellWindow(window)
{
    return (window.surface.windowFlags & WaylandSurface.BypassWindowManager) &&
            window.surface.windowProperties.role !== Compositor.ApplicationWindowRole;
}

function windowAdded(window)
{
    var parent;

    // Determine the correct parent
    switch (window.surface.windowProperties.role) {
    case Compositor.LockScreenRole:
        parent = root.layers.lock;
        break;
    case Compositor.OverlayWindowRole:
        parent = root.layers.overlay;
        break;
    case Compositor.DialogWindowRole:
        parent = root.layers.dialogs;
        break;
    case Compositor.FullScreenWindowRole:
        parent = root.layers.fullScreen;
        break;
    case Compositor.PanelWindowRole:
        parent = root.layers.panels;
        break;
    case Compositor.PopupWindowRole:
        parent = root.layers.panels;
        break;
    case Compositor.NotificationsWindowRole:
        parent = root.layers.notifications;
        break;
    case Compositor.DesktopWindowRole:
        parent = root.layers.desktop;
        break;
    case Compositor.BackgroundWindowRole:
        parent = root.layers.background;
        break;
    default:
        parent = root.layers.windows;
        break;
    }

    // Determine the correct wrapper for this window
    var componentUrl = isShellWindow(window)
            ? "ShellWindow.qml"
            : "ClientWindow.qml";

    // Create the wrapper
    var component = Qt.createComponent(componentUrl);
    if (component.status !== Component.Ready) {
        console.error("Couldn't create window wrapper:\n",
                      component.errorString());
        return;
    }
    var container = component.createObject(parent, {window: window});

    // Center globally modal dialogs and show overlay
    if (container.role === Compositor.DialogWindowRole) {
        window.x = root.x + (root.width - window.width) / 2;
        window.y = root.y + (root.height - window.height) / 2;
        root.layers.dialogs.overlay.opacity = 1.0;
    }

    // Run map animation
    if (window.runMapAnimation)
        window.runMapAnimation();

    // Add to the client window list
    if (windowList == null)
        windowList = new Array(0);
    windowList.push(container);
}

function windowRemoved(window)
{
    // Hide globally modal dialogs overlay
    if (window.surface.windowProperties.role)
        root.layers.dialogs.overlay.opacity = 0.0;

    // Run unmap animation
    if (window.runUnmapAnimation)
        window.runUnmapAnimation();
}

function windowDestroyed(window)
{
    // Run destroy animation
    if (window.runDestroyAnimation)
        window.runDestroyAnimation();
}

function windowRemoved(window)
{
    // Don't take this window into account anymore
    for (var i = 0; i < windowList.length; ++i) {
        if (windowList[i] === window) {
            windowList.splice(i, 1);
            break;
        }
    }

    // Destroy
    compositor.destroyWindow(window);
}
