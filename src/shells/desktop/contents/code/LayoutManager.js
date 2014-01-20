/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
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

.pragma library

var layout;
var root;
var view;

function insertBefore(item1, item2) {
    if (item1 === item2)
        return;
    var removed = new Array();

    var child;

    var i;
    for (i = layout.children.length - 1; i >= 0; --i) {
        child = layout.children[i];
        removed.push(child);
        child.parent = root;

        if (child === item1)
            break;
    }

    item2.parent = layout;

    var j;
    for (j = removed.length - 1; j >= 0; --j)
        removed[j].parent = layout;
    return i;
}

function insertAfter(item1, item2) {
    if (item1 === item2)
        return;
    var removed = new Array();

    var child;

    var i;
    for (i = layout.children.length - 1; i >= 0; --i) {
        child = layout.children[i];
        if (child === item1)
            break;

        removed.push(child);
        child.parent = root;
    }

    item2.parent = layout;

    var j;
    for (j = removed.length - 1; j >= 0; --j)
        removed[j].parent = layout;
    return i;
}

function insertAtIndex(item, position) {
    var removedItems = new Array();

    var i;
    for (i = position; i < layout.children.length; ++i) {
        var child = layout.children[position];
        child.parent = root;
        removedItems.push(child);
    }

    item.parent = layout;
    for (var i in removedItems)
        removedItems[i].parent = layout;
}

function insertAtCoordinates(item, x, y) {
    var child = layout.childAt(x, y);

    if (!child || child === item)
        return -1;
    item.parent = root;

    //Shell.Types.Vertical = 2
    if ((view.formFactor === 2 && y < child.y + child.height/2) ||
            (view.formFactor !== 2 && x < child.x + child.width/2)) {
        return insertBefore(child, item);
    } else {
        return insertAfter(child, item);
    }
}
