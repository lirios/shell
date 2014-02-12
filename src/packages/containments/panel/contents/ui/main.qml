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
import Hawaii.Shell 1.0 as Shell
import "../code/LayoutManager.js" as LayoutManager

DropArea {
    property Item configButton
    property Item dragOverlay

    id: root
    keys: ["application/x-hawaiishell-element"]
    onEntered: LayoutManager.insertAtCoordinates(dndSpacer, drag.x, drag.y)
    onPositionChanged: LayoutManager.insertAtCoordinates(dndSpacer, drag.x, drag.y)
    onExited: dndSpacer.parent = root
    onDropped: {
        if (drop.proposedAction === Qt.MoveAction || drop.proposedAction === Qt.CopyAction) {
            if (drop.keys === keys)
                drop.acceptProposedAction();
        }
    }

    Layout.minimumWidth: currentLayout.Layout.minimumWidth
    Layout.maximumWidth: currentLayout.Layout.maximumWidth
    Layout.preferredWidth: currentLayout.Layout.preferredWidth

    GridLayout {
        property bool isHorizontal: panel.formFactor === Shell.Types.Horizontal

        id: currentLayout
        anchors {
            left: parent.left
            top: parent.top
            right: isHorizontal && configButton ? configButton.left : parent.right
            bottom: isHorizontal ? parent.bottom : (configButton ? configButton.top : parent.bottom)
        }
        rows: 1
        columns: 1
        flow: isHorizontal ? GridLayout.TopToBottom : GridLayout.LeftToRight
    }

    Item {
        id: lastSpacer
        parent: currentLayout

        Layout.fillWidth: true
        Layout.fillHeight: true
    }

    Item {
        id: dndSpacer
        width: panel.formFactor === Shell.Types.Vertical ? currentLayout.width : 40
        height: panel.formFactor === Shell.Types.Vertical ? 40 : currentLayout.height
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
                if (panel.formFactor !== Shell.Types.Vertical)
                    return element && element.minimumWidth > 0 ? element.minimumWidth : root.height;
                return root.width;
            }
            Layout.minimumHeight: {
                if (panel.formFactor === Shell.Types.Vertical)
                    return element && element.minimumHeight > 0 ? element.minimumHeight : root.width;
                return root.height;
            }

            Layout.preferredWidth: {
                if (panel.formFactor !== Shell.Types.Vertical)
                    return element && element.implicitWidth > 0 ? element.implicitWidth : root.height;
                return root.width;
            }
            Layout.preferredHeight: {
                if (panel.formFactor === Shell.Types.Vertical)
                    return element && element.implicitHeight > 0 ? element.implicitHeight : root.width;
                return root.height;
            }

            Layout.maximumWidth: {
                if (panel.formFactor !== Shell.Types.Vertical)
                    return element && element.maximumWidth > 0 ? element.maximumWidth : (Layout.fillWidth ? -1 : root.height);
                return Layout.fillHeight ? -1 : root.width;
            }
            Layout.maximumHeight: {
                if (panel.formFactor === Shell.Types.Vertical)
                    return element && element.maximumHeight > 0 ? element.maximumHeight : (Layout.fillHeight ? -1 : root.width);
                return Layout.fillWidth ? -1 : root.height;
            }

            Layout.fillWidth: element && element.fillWidth
            Layout.fillHeight: element && element.fillHeight
        }
    }

    Connections {
        target: panel
        onElementAdded: {
            addElement(element);
        }
    }

    Connections {
        target: panel
        onConfiguringChanged: makeConfigurable()
    }

    Component.onCompleted: {
        LayoutManager.view = panel;
        LayoutManager.root = root;
        LayoutManager.layout = currentLayout;
        //LayoutManager.restore();
        makeConfigurable();
    }

    function addElement(element) {
        var container = elementComponent.createObject(root);

        if (element.fillWidth)
            lastSpacer.parent = root;

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
