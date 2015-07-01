/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0

ListModel {
    id: root

    property Item workspace: null

    ListElement {
        name: "PresentWindowsGrid"
        file: "effects/presentwindowsgrid/PresentWindowsGrid.qml"
        instance: 0
        running: false
    }

    ListElement {
        name: "RevealDesktop"
        file: "effects/revealdesktop/RevealDesktop.qml"
        instance: 0
        running: false
    }

    function isRunning(name) {
        var i;
        for (i = 0; i < root.count; i++) {
            // Find the effect
            var object = root.get(i);
            if (object.name !== name)
                continue;

            return object.running;
        }

        return false;
    }

    function stop() {
        // End all effects
        var i;
        for (i = 0; i < root.count; i++)
            root.get(i).end();
    }

    function toggle(name) {
        var i;
        for (i = 0; i < root.count; i++) {
            // Find the effect
            var object = root.get(i);
            if (object.name !== name)
                continue;

            // Instantiate the effect only once
            if (!object.instance) {
                var component = Qt.createComponent(object.file);
                if (component.status !== Component.Ready) {
                    console.error("Error loading effect " + name + ": " + component.errorString());
                    return;
                }

                object.instance = component.createObject(compositorRoot, {
                                                             "workspace": workspace
                                                         });
            }

            // Run or end
            if (object.running) {
                object.instance.end();
                object.running = false;
            } else {
                object.instance.run();
                object.running = true;
            }
            return;
        }
    }

    function run(name) {
        var i;
        for (i = 0; i < root.count; i++) {
            // Find the effect
            var object = root.get(i);
            if (object.name !== name)
                continue;

            // Don't run twice
            if (object.running)
                return;

            // Instantiate the effect only once
            if (!object.instance) {
                var component = Qt.createComponent(object.file);
                if (component.status !== Component.Ready) {
                    console.error("Error loading effect " + name + ": " + component.errorString());
                    return;
                }

                object.instance = component.createObject(compositorRoot, {
                                                             "workspace": workspace
                                                         });
            }

            // Run
            object.instance.run();
            object.running = true;
            return;
        }
    }

    function addWindow(window) {
        var i;
        for (i = 0; i < root.count; i++) {
            // Find the effect
            var object = root.get(i);
            if (!object.running || !object.instance)
                continue;

            // Add window only to running effects
            if (object.instance.addWindow !== undefined)
                object.instance.addWindow(window);
        }
    }

    function removeWindow(window) {
        var i;
        for (i = 0; i < root.count; i++) {
            // Find the effect
            var object = root.get(i);
            if (!object.running || !object.instance)
                continue;

            // Remove window from running effects
            if (object.instance.removeWindow !== undefined)
                object.instance.removeWindow(window);
        }
    }

    function end(name) {
        var i;
        for (i = 0; i < root.count; i++) {
            // Find the effect
            var object = root.get(i);
            if (object.name !== name)
                continue;

            // Stop the effect
            if (!object.instance)
                console.error("Effect " + name + " was not instantiated before calling end()");
            else
                object.instance.end();
            object.running = false;
            return;
        }
    }
}
