/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

var windowList = new Array(0);
var activeWindow = null;

/*
 * Main procedures
 */

function surfaceMapped(surface) {
    // Get the first view and if it has a role property than this
    // is definitely a shell window
    var firstView = compositor.firstViewOf(surface);
    if (typeof(firstView.role) == "undefined") {
        console.debug("Application surface", surface, "mapped");
        console.debug("\tappId:", surface.className);
        console.debug("\ttitle:", surface.title);
        console.debug("\tsize:", surface.size.width + "x" + surface.size.height);
    } else {
        console.debug("Shell surface", surface, "mapped");
        console.debug("\trole:", firstView.role);
        console.debug("\tsize:", surface.size.width + "x" + surface.size.height);
    }
    switch (surface.windowType) {
    case WaylandQuickSurface.Toplevel:
        console.debug("\ttype: Toplevel");
        break;
    case WaylandQuickSurface.Popup:
        console.debug("\ttype: Popup");
        break;
    case WaylandQuickSurface.Transient:
        console.debug("\ttype: Transient");
        break;
    default:
        console.debug("\ttype: Unknown");
        break;
    }

    // Call a specialized method to deal with application or
    // shell windows
    if (typeof(firstView.role) == "undefined")
        mapApplicationSurface(surface);
    else
        mapShellSurface(surface, firstView);
}

function surfaceUnmapped(surface) {
    // Get the first view and if it has a role property than this
    // is definitely a shell window
    var firstView = compositor.firstViewOf(surface);
    if (typeof(firstView.role) == "undefined") {
        console.debug("Application surface", surface, "unmapped");
        console.debug("\tappId:", surface.className);
        console.debug("\ttitle:", surface.title);
    } else {
        console.debug("Shell surface", surface, "unmapped");
        console.debug("\trole:", firstView.role);
        console.debug("\tsize:", surface.size.width + "x" + surface.size.height);
    }

    // Call a specialized method to deal with application or
    // shell windows
    if (typeof(firstView.role) == "undefined")
        unmapApplicationSurface(surface);
    else
        unmapShellSurface(surface);
}

function surfaceDestroyed(surface) {
    console.debug("Surface", surface, "destroyed");

    // Remove surface from model
    var i;
    for (i = 0; i < surfaceModel.count; i++) {
        var entry = surfaceModel.get(i);

        if (entry.surface === surface) {
            // Destroy window representation and
            // remove the surface from the model
            if (entry.window.chrome)
                entry.window.chrome.destroy();
            entry.window.destroy();
            surfaceModel.remove(i, 1);
            break;
        }
    }
}

/*
 * Map surfaces
 */

function mapApplicationSurface(surface) {
    // We get mapped/unmapped signals all the time for example when a
    // workspace is selected, for all the surfaces in the previous workspace
    // an unmapped signal is emitted; so we need to figure out if a
    // representation for the surface was already created and exit in that case
    var i;
    for (i = 0; i < surfaceModel.count; i++) {
        var entry = surfaceModel.get(i);

        if (entry.surface === surface)
            return;
    }

    // Create surface item
    var componentName = "ToplevelWindow.qml";
    switch (surface.windowType) {
    case WaylandQuickSurface.Popup:
        componentName = "PopupWindow.qml";
        break;
    case WaylandQuickSurface.Transient:
        componentName = "TransientWindow.qml";
        break;
    default:
        break;
    }
    var component = Qt.createComponent(componentName);
    if (component.status !== Component.Ready) {
        console.error(component.errorString());
        return;
    }

    // Window position
    var pos = Qt.point(0, 0);

    // Request a view for this output (Items cannot be shared between
    // windows so a new one is created on demand)
    var child = compositor.viewForOutput(surface, _greenisland_output);

    // Create and setup window container
    var window = component.createObject(compositorRoot, {"child": child});
    window.child.parent = window;
    window.child.touchEventsEnabled = true;
    window.setSize();

    // z-order
    if (surface.windowType === WaylandQuickSurface.Toplevel) {
        window.z = windowList.length;
        windowList.push(window);
        activeWindow = window;
    }

    // Transient parent view
    var transientParentView = null;
    if (surface.windowType === WaylandQuickSurface.Popup ||
            surface.windowType === WaylandQuickSurface.Transient) {
        // The parent could be a shell window instead of an application window,
        // this is the case for context menus of the desktop view.
        // We get the first view and see if it belongs to a shell window otherwise
        // a view for this output is requested.
        var parentFirstView = compositor.firstViewOf(surface.transientParent);
        if (typeof(parentFirstView.role) == "undefined")
            transientParentView = compositor.viewForOutput(surface.transientParent, _greenisland_output);
        else
            transientParentView = parentFirstView;
        console.log(parentFirstView, "vs", transientParentView);
    }

    // Determine window position
    switch (surface.windowType) {
    case WaylandQuickSurface.Toplevel:
        if (surface.state === QuickSurface.Normal) {
            pos = compositor.calculateInitialPosition(surface);
            surface.globalPosition = pos;
        } else {
            pos = surface.globalPosition;
        }
        pos = _greenisland_output.mapToOutput(pos);
        break;
    case WaylandQuickSurface.Popup:
        // Move popups relative to parent window
        pos.x = surface.transientOffset.x;
        pos.y = surface.transientOffset.y;
        surface.globalPosition = Qt.point(transientParentView.surface.globalPosition.x + pos.x,
                                          transientParentView.surface.globalPosition.y + pos.y);
        break;
    case WaylandQuickSurface.Transient:
        // Center transient windows
        pos.x = (transientParentView.width - window.width) / 2;
        pos.y = (transientParentView.height - window.height) / 2;
        surface.globalPosition = Qt.point(transientParentView.surface.globalPosition.x + pos.x,
                                          transientParentView.surface.globalPosition.y + pos.y);
        break;
    default:
        break;
    }

    // Move window
    window.x = pos.x;
    window.y = pos.y;

    // Reparent and give focus
    if (surface.windowType === WaylandQuickSurface.Toplevel)
        window.parent = compositorRoot.screenView.currentWorkspace;
    else
        window.parent = transientParentView.parent;
    window.child.takeFocus();

    // Set transient children so that the parent can be desaturated
    if (surface.windowType === WaylandQuickSurface.Transient)
        transientParentView.parent.transientChildren = window;
    // Set popup child to enable dim effect
    else if (surface.windowType === WaylandQuickSurface.Popup)
        transientParentView.parent.popupChild = window;

    // Log coordinates for debugging purpose
    console.debug("\tposition:", window.x + "," + window.y);
    console.debug("\tscreen:", compositorRoot.screenView.name);

    // Run map animation
    if (typeof(window.runMapAnimation) != "undefined")
        window.runMapAnimation();

    // Add surface to the model
    surfaceModel.append({"surface": surface, "window": window});
}

function mapShellSurface(surface, child) {
    // Shell surfaces have only one view which is passed to us
    // as an argument, check whether it's a view for this output
    // or not
    if (child.output !== _greenisland_output)
        return;

    // Create surface item
    var component = Qt.createComponent("WaylandShellWindow.qml");
    if (component.status !== Component.Ready) {
        console.error(component.errorString());
        return;
    }

    // Create and setup window container
    var window = component.createObject(compositorRoot, {"child": child});
    window.child.parent = window;
    window.child.touchEventsEnabled = true;
    window.width = surface.size.width;
    window.height = surface.size.height;

    // Set initial position
    window.x = window.y = 0;

    // Set appropriate parent
    switch (child.role) {
    case ShellWindowView.SplashRole:
        window.parent = compositorRoot.screenView.layers.splash;
        break;
    case ShellWindowView.DesktopRole:
    case ShellWindowView.DashboardRole:
        window.parent = compositorRoot.screenView.layers.desktop;
        break;
    case ShellWindowView.PanelRole:
    case ShellWindowView.ConfigRole:
        window.parent = compositorRoot.screenView.layers.panels;
        break;
    case ShellWindowView.OverlayRole:
        window.parent = compositorRoot.screenView.layers.overlays;
        break;
    case ShellWindowView.NotificationRole:
        window.parent = compositorRoot.screenView.layers.notifications;
        break;
    case ShellWindowView.LockRole:
        window.parent = compositorRoot.screenView.layers.lock;
        break;
    default:
        window.parent = compositorRoot.screenView.layers.desktop;
        break;
    }

    // Log coordinates for debugging purpose
    console.debug("\tposition:", window.x + "," + window.y);
    console.debug("\tscreen:", compositorRoot.screenView.name);

    // Add surface to the model
    surfaceModel.append({"surface": surface, "window": window});
}

/*
 * Unmap surfaces
 */

function unmapApplicationSurface(surface) {
    // Find window representation
    var i, window = null;
    for (i = 0; i < surfaceModel.count; i++) {
        var entry = surfaceModel.get(i);

        if (entry.surface === surface) {
            window = entry.window;
            break;
        }
    }
    if (!window)
        return;

    // Remove from z-order list
    for (i = 0; i < windowList.length; ++i) {
        if (windowList[i] === window) {
            windowList.splice(i, 1);
            break;
        }
    }

    // Unset transient children so that the parent can go back to normal
    // and also bring to front
    if (surface.windowType === WaylandQuickSurface.Transient) {
        var transientParentView = compositor.viewForOutput(surface.transientParent, _greenisland_output);
        transientParentView.parent.transientChildren = null;
        moveFront(transientParentView.parent);
    }

    // Looks like popup surfaces for Qt applications are never destroyed,
    // this means that the next time the surface is mapped we'll see it
    // in the surface model and don't create a window representation, hence
    // we destroy the surface item when it's unmapped
    if (surface.windowType === WaylandQuickSurface.Popup) {
        if (window.chrome)
            window.chrome.destroy();
        window.destroy();
        surfaceModel.remove(i, 1);
    }
}

function unmapShellSurface(surface) {
}

/*
 * Window management
 */

function moveFront(window) {
    // Ignore if the window is already the highest in the stacking order
    if (window.z == windowList.length - 1) {
        window.child.takeFocus();
        return;
    }

    console.debug("moveFront[" + windowList.length + "] for", window + ":");

    var initialZ = window.z
    var i;
    console.debug("\ti =", initialZ + 1, "; i <", windowList.length);
    for (i = initialZ + 1; i < windowList.length; ++i) {
        windowList[i].z = window.z;
        console.log("----", windowList[i], windowList[i].child, windowList[i].child.focus);
        windowList[i].child.focus = false;
        console.debug("\t" + windowList[i], "z:", windowList[i].z);
        window.z = i;
    }
    console.debug("\t" + window, "z:", window.z);

    windowList.splice(initialZ, 1);
    windowList.push(window);
    window.parent.parent.selectWorkspace(window.parent);
    window.child.takeFocus();
    activeWindow = window;
}

function getActiveWindowIndex() {
    if (!activeWindow)
        return -1;
    return windowList.indexOf(activeWindow);
}
