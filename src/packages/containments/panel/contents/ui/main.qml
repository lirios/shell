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

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Shell 1.0
import Hawaii.Shell.Core 1.0
import "../code/LayoutManager.js" as LayoutManager

DropArea {
    property Item configButton
    property Item dragOverlay

    property bool isHorizontal: Containment.formFactor !== Types.Vertical

    id: root
    keys: ["application/x-hawaiishell-element"]
    /*
    onEntered: LayoutManager.insertAtCoordinates(dndSpacer, drag.x, drag.y)
    onPositionChanged: LayoutManager.insertAtCoordinates(dndSpacer, drag.x, drag.y)
    onExited: dndSpacer.parent = root
    onDropped: {
        if (drop.proposedAction === Qt.MoveAction || drop.proposedAction === Qt.CopyAction) {
            if (drop.keys === keys)
                drop.acceptProposedAction();
        }
    }
*/
    onWidthChanged: containmentSizeSyncTimer.restart()
    onHeightChanged: containmentSizeSyncTimer.restart()
    onConfigButtonChanged: containmentSizeSyncTimer.restart()

    Layout.minimumWidth: currentLayout.Layout.minimumWidth
    Layout.maximumWidth: currentLayout.Layout.maximumWidth
    Layout.preferredWidth: currentLayout.Layout.preferredWidth

    Layout.minimumHeight: currentLayout.Layout.minimumHeight
    Layout.maximumHeight: currentLayout.Layout.maximumHeight
    Layout.preferredHeight: currentLayout.Layout.preferredHeight

    GridLayout {
        property bool isLayoutHorizontal

        id: currentLayout
        /*
        anchors {
            left: parent.left
            top: parent.top
            right: isHorizontal && configButton ? configButton.left : parent.right
            bottom: isHorizontal ? parent.bottom : (configButton ? configButton.top : parent.bottom)
        }
*/
        rows: 1
        columns: 1
        flow: isHorizontal ? GridLayout.TopToBottom : GridLayout.LeftToRight

        Layout.preferredWidth: {
            var width = 0;
            for (var i = 0; i < currentLayout.children.length; ++i) {
                if (currentLayout.children[i].Layout)
                    width += Math.max(currentLayout.children[i].Layout.minimumWidth, currentLayout.children[i].Layout.preferredWidth);
            }
            return width;
        }
        Layout.preferredHeight: {
            var height = 0;
            for (var i = 0; i < currentLayout.children.length; ++i) {
                if (currentLayout.children[i].Layout)
                    height += Math.max(currentLayout.children[i].Layout.minimumHeight, currentLayout.children[i].Layout.preferredHeight);
            }
            return height;
        }
    }

    Item {
        id: lastSpacer
        parent: currentLayout

        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    Item {
        id: dndSpacer
        width: root.Containment.formFactor === Types.Vertical ? currentLayout.width : 40
        height: root.Containment.formFactor === Types.Vertical ? 40 : currentLayout.height
    }

    Component {
        id: elementComponent

        Item {
            id: elementContainer
            transform: Translate {
                id: translateTransform
            }
            visible: false
            onXChanged: d.coordsChanged()
            onYChanged: d.coordsChanged()

            property Item element
            property bool animationsEnabled: false

            QtObject {
                id: d

                property int oldX: x
                property int oldY: y

                function coordsChanged() {
                    if (elementContainer.parent !== currentLayout) {
                        oldX = x;
                        oldY = y;
                        return;
                    }
                    translateTransform.x = oldX - x;
                    translateTransform.y = oldY - y;
                    translateAnimation.running = true;
                    oldX = x;
                    oldY = y;
                }
            }

            NumberAnimation {
                id: translateAnimation
                running: animationsEnabled
                easing.type: Easing.InOutQuad
                target: translateTransform
                properties: "x,y"
                to: 0
            }

            BusyIndicator {
                anchors.centerIn: parent
                z: 1000
                running: visible
                visible: element && element.busy
            }

            Layout.minimumWidth: {
                if (currentLayout.isLayoutHorizontal)
                    return element && element.Layout.minimumWidth > 0 ? element.Layout.minimumWidth : root.height;
                return root.width;
            }
            Layout.minimumHeight: {
                if (!currentLayout.isLayoutHorizontal)
                    return element && element.Layout.minimumHeight > 0 ? element.Layout.minimumHeight : root.width;
                return root.height;
            }

            Layout.preferredWidth: {
                if (currentLayout.isLayoutHorizontal)
                    return element && element.Layout.preferredWidth > 0 ? element.Layout.preferredWidth : root.height;
                return root.width;
            }
            Layout.preferredHeight: {
                if (!currentLayout.isLayoutHorizontal)
                    return element && element.Layout.preferredHeight > 0 ? element.Layout.preferredHeight : root.width;
                return root.height;
            }

            Layout.maximumWidth: {
                if (currentLayout.isLayoutHorizontal) {
                    if (element && element.Layout.maximumWidth > 0)
                        return element.Layout.maximumWidth;
                    else if (Layout.fillWidth)
                        return root.width;
                }
                return root.height;
            }
            Layout.maximumHeight: {
                if (!currentLayout.isLayoutHorizontal) {
                    if (element && element.Layout.maximumHeight > 0)
                        return element.Layout.maximumHeight;
                    else if (Layout.fillHeight)
                        return root.height;
                }
                return root.width;
            }

            Layout.fillWidth: element && element.Layout.fillWidth
            Layout.onFillWidthChanged: {
                if (root.Containment.formFactor !== Types.Vertical)
                    checkLastSpacer();
            }

            Layout.fillHeight: element && element.Layout.fillHeight
            Layout.onFillHeightChanged: {
                if (root.Containment.formFactor === Types.Vertical)
                    checkLastSpacer();
            }
        }
    }

    Timer {
        id: containmentSizeSyncTimer
        interval: 150
        onTriggered: {
            currentLayout.x = 0;
            currentLayout.y = 0;
            currentLayout.width = root.width - (isHorizontal && configButton && configButton.visible ? configButton.width : 0);
            currentLayout.height = root.height - (!isHorizontal && configButton && configButton.visible ? configButton.height : 0);
            currentLayout.isLayoutHorizontal = isHorizontal;
        }
    }

    Connections {
        target: panel
        onImmutableChanged: containmentSizeSyncTimer.restart()
        onElementAdded: addElement(element)
        onConfiguringChanged: makeConfigurable()
    }

    Containment.onFormFactorChanged: containmentSizeSyncTimer.restart()

    Component.onCompleted: {
        LayoutManager.view = panel;
        LayoutManager.root = root;
        LayoutManager.layout = currentLayout;
        //LayoutManager.restore();
        makeConfigurable();
    }

    function addElement(element) {
        var container = elementComponent.createObject(root);

        element.parent = container;
        container.element = element;
        element.anchors.fill = container;
        element.visible = true;
        container.visible = true;
        container.animationsEnabled = true;

        // Replace drag & drop spacer if available
        if (dndSpacer.parent === currentLayout) {
            LayoutManager.insertBefore(dndSpacer, container);
            dndSpacer.parent = root;
            return;
        }

        // Enqueue after spacer if any
        if (lastSpacer.parent === currentLayout)
            LayoutManager.insertBefore(lastSpacer, container);
        else
            container.parent = currentLayout;

        if (element.Layout.fillWidth)
            lastSpacer.parent = root;
    }

    function checkLastSpacer() {
        lastSpacer.parent = root

        var expands = false;
        var container;
        var item;

        if (isHorizontal) {
            for (container in currentLayout.children) {
                item = currentLayout.children[container];
                if (item.Layout && item.Layout.fillWidth)
                    expands = true;
            }
        } else {
            for (container in currentLayout.children) {
                item = currentLayout.children[container];
                if (item.Layout && item.Layout.fillHeight)
                    expands = true;
            }
        }

        if (!expands)
            lastSpacer.parent = currentLayout
    }

    function makeConfigurable() {
        if (panel.immutable) {
            if (dragOverlay)
                dragOverlay.destroy();
            return;
        }

        if (panel.configuring) {
            var component = Qt.createComponent("ConfigOverlay.qml");
            dragOverlay = component.createObject(root);
            component.destroy();
        } else if (dragOverlay) {
            dragOverlay.destroy();
        }
    }
}
