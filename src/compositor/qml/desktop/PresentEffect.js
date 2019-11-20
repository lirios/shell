/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2015 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

// Save original layout here in order to restore it later
var originalLayout = {};

// Save chromes that are created
var chromes = {};

function spreadWindows() {
    var workspaceWidth = workspace.width - desktop.margins.left - desktop.margins.right;
    var workspaceHeight = workspace.height - desktop.margins.top - desktop.margins.bottom;
    var spacing = calcSpacing();

    var layout = layoutWindows();

    //console.debug("layout", output.model, JSON.stringify(layout));

    var rowHeight = layout.rowHeight;
    var totalHeight = (rowHeight + spacing) * layout.rows.length + spacing;
    var offsetY = (workspaceHeight - totalHeight) / 2;

    layout.rows.forEach(function(row) {
        //console.debug("row", output.model, JSON.stringify(row));

        var offsetX = (workspaceWidth - row.width) / 2;

        row.windows.forEach(function(pos) {
            var entry = workspace.visibleChildren[pos.index];

            // Calculate position and size
            var x = output.position.x + pos.x + ((workspaceWidth - row.width) / 2);
            var y = output.position.y + pos.y + ((workspaceHeight - totalHeight) / 2);
            var w = entry.width * pos.scale;
            var h = entry.height * pos.scale;

            //console.debug(output.model, "-", entry.window.appId, "\"" + entry.window.title + "\"", x.toFixed(2) + "," + y.toFixed(2), w.toFixed(2) + "x" + h.toFixed(2));

            // Create the chrome
            var chrome = chromeComponent.createObject(workspace, {"view": entry});
            chrome.selected.connect(function(view) {
                workspace.effectStopped("present");
                view.shellSurfaceItem.takeFocus();
            });
            chrome.closed.connect(function(view) {
                workspace.effectStopped("present");
                view.window.sendClose();
            });
            chromes[entry] = chrome;

            // Save original position and size (compositor space coordinates)
            originalLayout[entry] = {
                "x": entry.window.moveItem.x, "y": entry.window.moveItem.y,
                "width": entry.width, "height": entry.height,
                "scale": entry.scale,
                "transformOrigin": entry.transformOrigin
            };

            // Move and resize
            entry.shellSurfaceItem.output.screenView.locked = true;
            entry.transformOrigin = Item.TopLeft;
            entry.window.moveItem.animateTo(x, y);
            entry.scale = pos.scale;
        });
    });
}

function restoreWindows() {
    // This loop needs to run on all shell surfaces so we make their views
    // visible again on restore
    for (var i = 0; i < workspace.visibleChildren.length; i++) {
        var entry  = workspace.visibleChildren[i];

        var pos = originalLayout[entry];
        if (pos !== undefined) {
            // Restore to the original position and size
            entry.window.moveItem.animateTo(pos.x, pos.y);
            entry.scale = pos.scale;
            entry.shellSurfaceItem.output.screenView.locked = false;
            entry.transformOrigin = pos.transformOrigin;
            delete originalLayout[entry];

            // Delete the chrome
            chromes[entry].destroy();
            delete chromes[entry];
        }

        // Show entry again
        entry.visible = true;
    }
}

function layoutWindows() {
    var rows = 1;
    var layout;

    // Reset original layout dictionary
    originalLayout = {};

    do {
        layout = tryLayout(rows++);
    } while (layout === undefined);

    return layout;
}

function tryLayout(rows) {
    var workspaceWidth = workspace.width - desktop.margins.left - desktop.margins.right;
    var workspaceHeight = workspace.height - desktop.margins.top - desktop.margins.bottom;
    var spacing = calcSpacing();

    var rowHeight = (workspaceHeight - spacing) / rows - spacing;
    var rowWidth = spacing;
    var row = 0;
    var rowList = [];
    var list = [];
    var i, entry;

    for (i = 0; i < workspace.visibleChildren.length; i++) {
        entry  = workspace.visibleChildren[i];
        if (!entry.window)
            continue;

        // Only top level windows
        if (entry.window.windowType !== Qt.Window)
            continue;

        // Consider only entries for this output
        if (!isEntryOnOutput(entry, output))
            continue;

        rowHeight = Math.min(rowHeight, entry.height);
    }

    for (i = 0; i < workspace.visibleChildren.length; i++) {
        entry  = workspace.visibleChildren[i];
        if (!entry.window)
            continue;

        // Only top level windows
        if (entry.window.windowType !== Qt.Window) {
            entry.visible = false;
            continue;
        }

        // Consider only entries for this output
        if (!isEntryOnOutput(entry, output)) {
            // A bit ugly because the window disappear on the other output
            // while it is still animating to the new position, but still...
            entry.visible = false;
            continue;
        }

        var scale = rowHeight / entry.height;
        var windowWidth = entry.width * scale;

        if (scale > 1)
            return undefined;

        if (rowWidth + windowWidth + spacing > workspaceWidth) {
            list.push({"width": rowWidth, "windows": rowList});
            rowWidth = spacing;
            rowList = [];
            row++;
        }

        rowList.push({
                         "x": rowWidth,
                         "y": spacing + (rowHeight + spacing) * row,
                         "scale": scale,
                         "index": i
                     });

        rowWidth += windowWidth + spacing;

        if (row >= rows)
            return undefined;
    }

    list.push({"width": rowWidth, "windows": rowList});

    return {"count": rows, "rows": list, "rowHeight": rowHeight};
}

/*
 * Utilities
 */

// Return true if window entry is inside the output
function isEntryOnOutput(entry, output) {
    var area = entry.width * entry.height;
    var outputWidth = output.geometry.width;
    var outputHeight = output.geometry.height;
    var x1 = Math.max(0, entry.x);
    var y1 = Math.max(0, entry.y);
    var x2 = Math.min(entry.x + entry.width, outputWidth);
    var y2 = Math.min(entry.y + entry.height, outputHeight);
    var w1 = Math.max(0, x2 - x1);
    var h1 = Math.max(0, y2 - y1);
    return w1 * h1 * 2 > area;
}

// Calculate spacing between windows based on how many of them there are
function calcSpacing() {
    var count = 0;
    var i, entry;

    for (i = 0; i < workspace.visibleChildren.length; i++) {
        entry  = workspace.visibleChildren[i];

        // Count only windows on this output
        if (isEntryOnOutput(entry, output))
            count++;
    }

    if (count <= 2)
        return 64 * output.scaleFactor;
    else if (count === 3)
        return 32 * output.scaleFactor;
    else
        return 16 * output.scaleFactor;
}
