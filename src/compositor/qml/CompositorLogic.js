/****************************************************************************
**
** Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

var windowList = null;
var indexes = null;

function addWindow(window)
{
    if (windowList == null)
        windowList = new Array(0);

    windowList.push(window);

    if (!root.normalLayout)
        relayout();
}

function removeWindow(window)
{
    for (var i = 0; i < windowList.length; ++i) {
        if (windowList[i] == window) {
            if (root.normalLayout)
                windowList.splice(i, 1);
            break;
        }
    }

    if (root.normalLayout)
        return;

    var index = windowList[i].index;
    var dim = Math.ceil(Math.sqrt(windowList.length));
    var maxY = Math.floor((windowList.length-1) / dim);

    var shrinking = Math.ceil(Math.sqrt(windowList.length - 1)) != dim;

    while (true) {
        var ix = index % dim;
        var iy = Math.floor(index / dim);

        if (shrinking) {
            if (iy > 0)
                --iy;
            else if (++ix == dim)
                break;
        } else {
            if (iy < maxY) {
                if (ix > 0)
                    --ix;
                else
                    ++iy;
            } else {
                ++ix;
            }
        }

        var next = iy * dim + ix;

        var currentIndex = indexes[index];
        var nextIndex = indexes[next];

        if (nextIndex == null)
            break;

        var temp = windowList[currentIndex];
        windowList[currentIndex] = windowList[nextIndex];
        windowList[currentIndex].index = currentIndex;
        windowList[nextIndex] = temp;

        index = next;
    }

    windowList.splice(indexes[index], 1);
    relayout();
}

function relayout()
{
    if (root.normalLayout) {
        restorelayout();
        return;
    }

    if (windowList === null || windowList.length == 0)
        return;

    var dim = Math.ceil(Math.sqrt(windowList.length));

    var cols = dim;
    var rows = Math.ceil(windowList.length / cols);

    var w = root.availableGeometry.width / dim;
    var h = root.availableGeometry.height / rows;

    var ix = 0;
    var iy = 0;
    var lastDim = 1;

    indexes = new Array(dim * dim);

    for (var i = 0; i < windowList.length; i++) {
        if (i > 0) {
            var currentDim = Math.ceil(Math.sqrt(i + 1));
            if (currentDim == lastDim) {
                if (iy < currentDim - 1) {
                    ++iy;
                    if (iy == currentDim - 1)
                        ix = 0;
                } else {
                    ++ix;
                }
            } else {
                iy = 0;
                ix = currentDim - 1;
            }
            lastDim = currentDim;
        }

        indexes[iy * dim + ix] = i;
        windowList[i].index = iy * dim + ix;

        var cx = (ix + 0.5) * w;
        if (cx < root.availableGeometry.x)
            cx += root.availableGeometry.x;
        var cy = (iy + 0.5) * h;
        if (cy > root.availableGeometry.y)
            cy += root.availableGeometry.y;

        windowList[i].targetScale = 0.98 * Math.min(w / windowList[i].width, h / windowList[i].height);

        windowList[i].originalX = windowList[i].targetX;
        windowList[i].originalY = windowList[i].targetY;
        windowList[i].targetX = (cx - windowList[i].width / 2);
        windowList[i].targetY = (cy - windowList[i].height / 2);
    }
}

function restorelayout()
{
    if (windowList === null || windowList.length == 0)
        return;

    for (var i = 0; i < windowList.length; i++) {
        windowList[i].targetScale = 1.0;

        windowList[i].targetX = windowList[i].originalX;
        windowList[i].targetY = windowList[i].originalY;
    }
}

/*
 * Window management
 */

function shellWindowAdded(window) {
    // Create container object
    var windowContainerComponent = Qt.createComponent("ShellWindowContainer.qml");
    var windowContainer = windowContainerComponent.createObject(root);

    // Set window parent
    window.parent = windowContainer;
    windowContainer.child = window;

    // Full opacity
    windowContainer.opacity = 1.0;

    // Set geometry
    windowContainer.width = window.width;
    windowContainer.height = window.height;
}

function windowAdded(window) {
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
    addWindow(windowContainer);

    // Full opacity
    windowContainer.opacity = 1.0;
    ////
    windowContainer.z = 2;

    var windowChromeComponent = Qt.createComponent("WindowChrome.qml");
    var windowChrome = windowChromeComponent.createObject(window);

    windowContainer.animationsEnabled = true;
    windowContainer.chrome = windowChrome;
}

function windowResized(window) {
    var windowContainer = window.parent;
    windowContainer.width = window.width;
    windowContainer.height = window.height;

    // Recalculate the layout
    relayout();
}

function windowDestroyed(window) {
    var windowContainer = window.parent;
    // TODO: Run an animation
}

function windowRemoved(compositor, window) {
    var windowContainer = window.parent;

    // Don't take this window into account anymore
    removeWindow(windowContainer);

    // Destroy
    windowContainer.chrome.destroy();
    windowContainer.destroy();
    compositor.destroyWindow(window);
}
