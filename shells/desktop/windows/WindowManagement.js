/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
 * Application windows
 */

function _printWindowInfo(window) {
    console.debug("\twindow:", window);
    console.debug("\tid:", window.id);
    console.debug("\tsurface:", window.surface);
    console.debug("\tappId:", window.appId);
    console.debug("\ttitle:", window.title);
    console.debug("\tposition:", window.x + "," + window.y);
    console.debug("\tsize:", window.size.width + "x" + window.size.height);
    switch (window.type) {
    case GreenIsland.ClientWindow.TopLevel:
        console.debug("\ttype: TopLevel");
        break;
    case GreenIsland.ClientWindow.Popup:
        console.debug("\ttype: Popup");
        break;
    case GreenIsland.ClientWindow.Transient:
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
    var component = null;
    switch (window.type) {
    case GreenIsland.ClientWindow.TopLevel:
        component = compositorRoot.topLevelWindowComponent;
        break;
    case GreenIsland.ClientWindow.Popup:
        component = compositorRoot.popupWindowComponent;
        break;
    case GreenIsland.ClientWindow.Transient:
        component = compositorRoot.transientWindowComponent;
        break;
    default:
        console.error("Unknown window type", window.type);
        return;
    }

    // Retrieve the view for this output
    var child = window.viewForOutput(_greenisland_output);

    // Determine the parent
    var parentItem = compositorRoot;
    switch (window.type) {
    case GreenIsland.ClientWindow.TopLevel:
        parentItem = window.fullScreen
                ? compositorRoot.screenView.layers.fullScreen
                : compositorRoot.screenView.currentWorkspace
        break;
    case GreenIsland.ClientWindow.Popup:
    case GreenIsland.ClientWindow.Transient:
        parentItem = window.parentViewForOutput(_greenisland_output).parent;
        break;
    default:
        break;
    }

    // Create and setup window container
    var item = component.createObject(parentItem, {"clientWindow": window, "child": child});
    item.child.parent = item;
    item.child.anchors.fill = Qt.binding(function() { return item });
/*
    item.child.parent = item.decoration.container;
    item.child.anchors.fill = Qt.binding(function() { return item.decoration.container; });
*/
    item.child.touchEventsEnabled = true;
    item.x = child.localX;
    item.y = child.localY;
    item.width = window.size.width;
    item.height = window.size.height;
    console.debug("\titem:", item);
    console.debug("\titem position:", item.x + "," + item.y);

    // Set transient children so that the parent can be desaturated
    if (window.type === GreenIsland.ClientWindow.Transient)
        parentItem.transientChildren = item;
    // Set popup child to enable dim effect
    else if (window.type === GreenIsland.ClientWindow.Popup)
        parentItem.popupChild = item;

    // Make it visible
    item.visible = true;

    // z-order and focus
    if (window.type === GreenIsland.ClientWindow.TopLevel) {
        item.z = windowList.length;
        window.activate();
    } else if (window.type === GreenIsland.ClientWindow.Transient) {
        item.child.takeFocus();
    }

    // Add window to current effect
    compositorRoot.addWindowToEffect(item);

    // Run map animation
    item.runMapAnimation();

    // Add surface to the model
    surfaceModel.append({"id": window.id, "window": window, "item": item, "surface": window.surface});
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

    // Forget window
    _forgetWindow(i, entry.window, entry.item, false);
}

function windowDestroyed(id) {
    // Find window representation
    var i, found = false, entry = null;
    for (i = 0; i < surfaceModel.count; i++) {
        entry = surfaceModel.get(i);

        if (entry.id === id) {
            found = true;
            break;
        }
    }
    if (!found)
        return;

    // Debug
    console.debug("Application window destroyed");
    _printWindowInfo(entry.window);

    // Forget window
    _forgetWindow(i, entry.window, entry.item, true);
}

/*
 * Window management
 */

function moveFront(window) {
    var initialZ = window.z;

    console.debug("moveFront[" + windowList.length + "] initialZ:", initialZ);
    console.debug("\twindow:", window);

    var i;
    for (i = initialZ + 1; i < windowList.length; ++i) {
        windowList[i].z = window.z;
        window.z = i;
        console.debug("\t\t-> above:", windowList[i].z, "selected:", window.z);
    }
    console.debug("\t\t-> new:", window.z);

    windowList.splice(initialZ, 1);
    windowList.push(window);

    //console.debug("\twindowList:", windowList);
    //console.debug("\twindowList.length:", windowList.length);

    // XXX: Full screen windows do not have workspace as a parent
    // hence calling selectWorkspace() here will result in an error
    // TODO: Handle this in a different way
    if (!window.clientWindow.fullScreen)
        window.parent.parent.selectWorkspace(window.parent);
    activeWindow = window;
    compositorRoot.activeWindow = activeWindow;

    // Give focus to the window
    window.child.takeFocus();
}

function _forgetWindow(i, window, item, destruction) {
    // Remove from model
    surfaceModel.remove(i);

    // Remove from z-order list
    var j, windowIndex = -1;
    if (window.type === GreenIsland.ClientWindow.TopLevel) {
        for (j = 0; j < windowList.length; j++) {
            if (windowList[j] === item) {
                windowList.splice(j, 1);
                windowIndex = j - 1;
                break;
            }
        }
    }

    // Remove window from effect
    compositorRoot.removeWindowFromEffect(item);

    // Do not update child because it is going away
    item.child.paintEnabled = false;

    // Run animation
    if (destruction)
        item.runDestroyAnimation();
    else
        item.runUnmapAnimation();

    // Activate previous top level window
    if (windowIndex >= 0)
        windowList[windowIndex].clientWindow.activate();

    // Unset transient children so that the parent can go back to normal
    // and also bring the parent to front
    if (window.type === GreenIsland.ClientWindow.Transient) {
        var parentWindow = window.parentWindow;
        if (parentWindow) {
            var parentItem = parentWindow.viewForOutput(_greenisland_output).parent;
            parentItem.transientChildren = null;
            parentItem.clientWindow.activate();
        }
    }
}

function getActiveWindowIndex() {
    if (!activeWindow)
        return -1;
    return windowList.indexOf(activeWindow);
}
