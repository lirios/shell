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

function surfaceMapped(surface) {
    // Assume application window role by default
    if (typeof(surface.windowProperties.role) == "undefined")
        surface.windowProperties.role = Compositor.ApplicationRole;

    if (surface.windowProperties.role === Compositor.ApplicationRole) {
        console.debug("Surface " + surface + " mapped (" +
                      "className: \"" + surface.className + "\", " +
                      "title: \"" + surface.title + "\"): " +
                      surface.size.width + "x" + surface.size.height);
    } else {
        console.debug("Surface " + surface + " mapped (role: " +
                      surface.windowProperties.role + "): " +
                      surface.size.width + "x" + surface.size.height);
    }

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
    var component = Qt.createComponent("WaylandClientWindow.qml");
    if (component.status === Component.Ready) {
        // Window position
        var pos = Qt.point(0, 0);

        // Determine window position
        switch (surface.windowType) {
        case WaylandQuickSurface.Toplevel:
            pos = compositor.calculateInitialPosition(surface);
            break;
        case WaylandQuickSurface.Popup:
        case WaylandQuickSurface.Transient:
            var transientParentView = compositor.firstViewOf(surface.transientParent);

            if (surface.windowType === WaylandQuickSurface.Popup) {
                // Move popups relative to parent window
                pos.x = transientParentView.parent.x + surface.transientOffset.x;
                pos.y = transientParentView.parent.y + surface.transientOffset.y;
            } else {
                // Center transient windows
                pos.x = transientParentView.parent.x + (transientParentView.width - window.width) / 2;
                pos.y = transientParentView.parent.y + (transientParentView.height - window.height) / 2;
            }
            break;
        default:
            break;
        }

        // Determine the screen based on coordinates
        var screenView = compositorRoot.screenViews.screenViewForCoordinates(pos.x, pos.y);

        // Child
        var child = compositor.firstViewOf(surface);

        // Create and setup window container
        var window = component.createObject(screenView.currentWorkspace, {"child": child});
        window.child.parent = window;
        window.child.touchEventsEnabled = true;
        window.width = surface.size.width;
        window.height = surface.size.height;

        // Move window
        switch (surface.windowType) {
        case WaylandQuickSurface.Toplevel:
            var newPos = mapFromItem(screenView, pos.x, pos.y);
            window.x = newPos.x;
            window.y = newPos.y;
            break;
        default:
            window.y = pos.x;
            window.y = pos.y;
            break;
        }

        // Run map animation
        if (typeof(window.runMapAnimation) != "undefined")
            window.runMapAnimation();

        // Add surface to the model
        surfaceModel.append({"surface": surface, "window": window});
    } else {
        // Bail out
        console.error(component.errorString());
    }
}

function surfaceUnmapped(surface) {
    if (surface.windowProperties.role === Compositor.ApplicationRole) {
        console.debug("Surface " + surface + " unmapped (" +
                      "className: \"" + surface.className + "\", " +
                      "title: \"" + surface.title + "\")");
    } else {
        console.debug("Surface " + surface + " mapped (role: " +
                      surface.windowProperties.role + "): " +
                      surface.size.width + "x" + surface.size.height);
    }

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
}

function surfaceDestroyed(surface) {
    console.debug("Surface " + surface + " destroyed");

    // Remove surface from model
    var i;
    for (i = 0; i < surfaceModel.count; i++) {
        var entry = surfaceModel.get(i);

        if (entry.surface === surface) {
            // Destroy window representation and
            // remove the surface from the model
            entry.window.destroy();
            surfaceModel.remove(i, 1);
            break;
        }
    }

    // Damage all surfaces
    //compositor.damageAll();
}
