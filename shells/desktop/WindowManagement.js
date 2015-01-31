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

function _printWindowInfo(window) {
    console.debug("\twindow:", window);
    console.debug("\tsurface:", window.surface);
    console.debug("\tappId:", window.appId);
    console.debug("\ttitle:", window.title);
    console.debug("\tposition:", window.x + "," + window.y);
    console.debug("\tsize:", window.size.width + "x" + window.size.height);
    switch (window.type) {
    case ClientWindow.TopLevel:
        console.debug("\ttype: TopLevel");
        break;
    case ClientWindow.Popup:
        console.debug("\ttype: Popup");
        break;
    case ClientWindow.Transient:
        console.debug("\ttype: Transient");
        break;
    default:
        break;
    }
    console.debug("\tparentWindow:", window.parentWindow);
    console.debug("\tscreen:", compositorRoot.screenView.name);
}

function windowMapped(window) {
    console.debug("Application window mapped");
    _printWindowInfo(window);

    // Create surface item
    var componentName = "ToplevelWindow.qml";
    switch (window.type) {
    case ClientWindow.Popup:
        componentName = "PopupWindow.qml";
        break;
    case ClientWindow.Transient:
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

    // Retrieve the view for this output
    var child = window.viewForOutput(_greenisland_output);

    // Determine the parent
    var parentItem = compositorRoot;
    switch (window.type) {
    case ClientWindow.TopLevel:
        parentItem = compositorRoot.screenView.currentWorkspace
        break;
    case ClientWindow.Popup:
    case ClientWindow.Transient:
        parentItem = window.parentWindow.viewForOutput(_greenisland_output).parent;
        break;
    default:
        break;
    }

    // Create and setup window container
    var item = component.createObject(parentItem, {"clientWindow": window, "child": child});
    item.child.parent = item;
    item.child.touchEventsEnabled = true;
    item.x = window.x;
    item.y = window.y;
    item.width = window.size.width;
    item.height = window.size.height;

    // Set transient children so that the parent can be desaturated
    if (window.type === ClientWindow.Transient)
        parentItem.transientChildren = item;
    // Set popup child to enable dim effect
    else if (window.type === ClientWindow.Popup)
        parentItem.popupChild = item;

    // z-order and focus
    if (window.type === ClientWindow.TopLevel) {
        item.z = windowList.push(item);
        activeWindow = item;
        compositorRoot.activeWindow = activeWindow;
        item.forceActiveFocus();
    }

    // Make it visible
    item.visible = true;

    // Add window to current effect
    compositorRoot.addWindowToEffect(item);

    // Run map animation
    if (item.runMapAnimation != undefined)
        item.runMapAnimation();

    // Add surface to the model
    surfaceModel.append({"window": window, "item": item, "surface": window.surface});
}

function windowUnmapped(window) {
    console.debug("Application window unmapped");
    _printWindowInfo(window);

    // Find window representation
    var i, item = null;
    for (i = 0; i < surfaceModel.count; i++) {
        var entry = surfaceModel.get(i);

        if (entry.window === window) {
            item = entry.item;
            break;
        }
    }
    if (!item)
        return;

    // Remove from model
    surfaceModel.remove(i);

    // Remove from z-order list
    if (window.type === ClientWindow.TopLevel) {
        for (i = 0; i < windowList.length; ++i) {
            if (windowList[i] === item) {
                windowList.splice(i, 1);
                break;
            }
        }
    }

    // Remove window from effect
    compositorRoot.removeWindowFromEffect(item);

    // Unset transient children so that the parent can go back to normal
    // and also bring to front
    if (window.type === ClientWindow.Transient) {
        var parentItem = window.parentWindow.viewForOutput(_greenisland_output).parent;
        parentItem.transientChildren = null;
        //parentItem.child.takeFocus();
    }

    // Destroy window representation
    item.destroy();
}

/*
 * Map surfaces
 */

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
 * Window management
 */

function moveFront(window) {
    var initialZ = Math.ceil(window.z);

    // Ignore if the window is already the highest in the stacking order
    if (initialZ == windowList.length - 1) {
        //window.child.takeFocus();
        return;
    }

    console.debug("moveFront[" + windowList.length + "] for", window + ":");

    var i;
    console.debug("\ti =", initialZ + 1, "; i <", windowList.length);
    for (i = initialZ + 1; i < windowList.length; ++i) {
        windowList[i].z = Math.ceil(window.z);
        if (windowList[i].child)
            windowList[i].child.focus = false;
        console.debug("\t" + windowList[i], "z:", Math.ceil(windowList[i].z));
        window.z = i;
    }
    console.debug("\t" + window, "z:", Math.ceil(window.z));

    windowList.splice(initialZ, 1);
    windowList.push(window);
    window.parent.parent.selectWorkspace(window.parent);
    activeWindow = window;
    compositorRoot.activeWindow = activeWindow;
}

function getActiveWindowIndex() {
    if (!activeWindow)
        return -1;
    return windowList.indexOf(activeWindow);
}
