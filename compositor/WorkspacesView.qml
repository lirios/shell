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

import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    readonly property Item currentWorkspace:  view ? view.currentItem : null
    readonly property int currentIndex: view ? view.currentIndex : -1
    property Item view

    signal workspaceAdded(int index)
    signal workspaceRemoved(int index);
    signal workspaceSelected(int index);

    id: root

    function add() {
        root.workspaceAdded(view.count);
    }

    function remove(index) {
        root.workspaceRemoved(index);
    }

    function select(index) {
        root.workspaceSelected(index);
    }

    function selectPrevious() {
        // Previous index (avoid overflow)
        var prevIndex = view.currentIndex - 1;
        if (prevIndex < 0)
            prevIndex = view.count - 1;
        if (prevIndex < 0)
            prevIndex = 0;

        // Select workspace
        select(prevIndex);
    }

    function selectNext() {
        // Next index (avoid overflow)
        var nextIndex = view.currentIndex + 1;
        if (nextIndex >= view.count)
            nextIndex = 0;

        // Select workspace
        select(nextIndex);
    }
}
