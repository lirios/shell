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
    var component = Qt.createComponent("WaylandWindow.qml");
    if (component.status === Component.Ready) {
        // Child
        var child = compositor.firstViewOf(surface);

        // Create and setup window container
        var window = component.createObject(root, {"child": child});
        window.child.parent = window;
        window.child.touchEventsEnabled = true;
        window.width = surface.size.width;
        window.height = surface.size.height;
        window.reparent();

        // Move window to its initial position
        var pos = compositor.calculateInitialPosition(surface);
        window.x = pos.x;
        window.y = pos.y;

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
}

function surfaceDestroyed(surface) {
    console.debug("Surface " + surface + " destroyed");

    // Remove surface from model
    var i;
    for (i = 0; i < surfaceModel.count; i++) {
        var entry = surfaceModel.get(i);

        if (entry.surface === surface) {
            entry.window.destroy();
            surfaceModel.remove(i, 1);
            break;
        }
    }

    // Damage all surfaces
    //compositor.damageAll();
}
