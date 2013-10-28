/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

var windowList = null;

function shellWindowAdded(window)
{
    // Create container object
    var windowContainerComponent = Qt.createComponent("ShellWindowContainer.qml");
    var windowContainer = windowContainerComponent.createObject(root);

    // Set window parent
    window.parent = windowContainer;
    windowContainer.child = window;

    // Full opacity
    windowContainer.opacity = 1.0;

    // Set geometry and z-index
    windowContainer.width = window.width;
    windowContainer.height = window.height;
    windowContainer.z = window.z;
    return;

    var windowChromeComponent = Qt.createComponent("ShellWindowChrome.qml");
    var windowChrome = windowChromeComponent.createObject(window);
    windowContainer.chrome = windowChrome;
}

function windowAdded(window)
{
    var windowContainerComponent = Qt.createComponent("WindowContainer.qml");
    var windowContainer = windowContainerComponent.createObject(root);

    window.parent = windowContainer;

    windowContainer.targetScale = 1.0;
    windowContainer.targetWidth = window.width;
    windowContainer.targetHeight = window.height;
    windowContainer.child = window;

    // Automatically give focus to new windows
    window.takeFocus();

    // Add to the client window list
    if (windowList == null)
        windowList = new Array(0);
    windowList.push(windowContainer);

    // Full opacity
    windowContainer.opacity = 1.0;

    // Set appropriate z-index
    windowContainer.z = 2;

    // Create window chrome to handle focus and enable animations
    var windowChromeComponent = Qt.createComponent("WindowChrome.qml");
    var windowChrome = windowChromeComponent.createObject(window);
    windowContainer.animationsEnabled = true;
    windowContainer.chrome = windowChrome;
}

function windowResized(window)
{
    var windowContainer = window.parent;
    windowContainer.width = window.width;
    windowContainer.height = window.height;
}

function windowDestroyed(window)
{
    var windowContainer = window.parent;
    // TODO: Run an animation
}

function windowRemoved(window)
{
    var windowContainer = window.parent;

    // Don't take this window into account anymore
    for (var i = 0; i < windowList.length; ++i) {
        if (windowList[i] == windowContainer) {
            windowList.splice(i, 1);
            break;
        }
    }

    // Destroy
    windowContainer.chrome.destroy();
    windowContainer.destroy();
    compositor.destroyWindow(window);
}
