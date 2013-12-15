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

function shellWindowMapped(window)
{
    // Do something according to window role
    switch (window.role) {
    case Compositor.PopupWindowRole:
        // Popup windows always take the focus
        window.takeFocus();
        break;
    case Compositor.DialogWindowRole:
        // Modal dialogs take focus and are centered on an overlay
        window.takeFocus();
        window.x = root.x + (root.width - window.width) / 2;
        window.y = root.y + (root.height - window.height) / 2;
        root.showModalOverlay();
        break;
    default:
        break;
    }

    // Run map animation
    if (window.runMapAnimation)
        window.runMapAnimation();
}

function shellWindowUnmapped(window)
{
    // Run unmap animation
    if (window.runUnmapAnimation)
        window.runUnmapAnimation();
}

function shellWindowDestroyed(window)
{
    // Run destroy animation
    if (window.runDestroyAnimation)
        window.runDestroyAnimation();

    // Hide modal overlay
    if (window.role === Compositor.DialogWindowRole)
        root.hideModalOverlay();
}

function windowAdded(window)
{
    // Automatically give focus to new windows
    window.takeFocus();

    // Run map animation
    if (window.runMapAnimation)
        window.runMapAnimation();

    // Add to the client window list
    if (windowList == null)
        windowList = new Array(0);
    windowList.push(window);
}

function windowUnmapped(window)
{
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
