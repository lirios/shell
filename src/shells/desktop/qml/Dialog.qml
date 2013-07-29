/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import QtQuick.Window 2.0
import Hawaii.Shell.Desktop 0.1
import Hawaii.Shell.Styles 0.1

ShellWindow {
    id: dialogWindow
    color: "transparent"

    default property alias content: container.children

    signal rejected()

    onRejected: dialogWindow.visible = false

    Item {
        id: root
        anchors.fill: parent
        opacity: dialogWindow.visible ? 1.0 : 0.0
        focus: true

        property Component style: Qt.createComponent("DialogStyle.qml", root)

        property QtObject __style: styleLoader.item

        Keys.onReleased: {
            // Emit the rejected signal automatically when ESC is released
            if (event.key === Qt.Key_Escape)
                dialogWindow.rejected();
        }

        Behavior on opacity {
            NumberAnimation { duration: 250; easing.type: Easing.OutQuad }
        }

        Loader {
            id: backgroundLoader
            anchors.fill: parent
            sourceComponent: root.__style ? root.__style.background : null
            onStatusChanged: {
                if (status === Loader.Error)
                    console.error("Failed to load Style for", root);
            }
            onLoaded: item.z = -1

            Loader {
                property Item contentItem: container

                id: dialogLoader
                anchors.centerIn: parent
                sourceComponent: root.__style ? root.__style.dialog : null
                onStatusChanged: {
                    if (status === Loader.Error)
                        console.error("Failed to load Style for", root);
                }
                onLoaded: {
                    item.z = 0;
                    item.width = Math.max(container.implicitWidth, container.width) +
                            root.__style.padding.left + root.__style.padding.right;
                    item.height = Math.max(container.implicitHeight, container.height) +
                            root.__style.padding.top + root.__style.padding.bottom;
                }

                Item {
                    property Item layoutItem: container.children.length === 1 ? container.children[0] : null

                    id: container
                    anchors.fill: parent
                    anchors.leftMargin: root.__style.padding.left
                    anchors.topMargin: root.__style.padding.top
                    anchors.rightMargin: root.__style.padding.right
                    anchors.bottomMargin: root.__style.padding.bottom
                    z: 1
                    focus: true
                    implicitWidth: container.childrenRect.width
                    implicitHeight: container.childrenRect.height
                }

                Loader {
                    property Item __item: root

                    id: styleLoader
                    sourceComponent: root.style
                    onStatusChanged: {
                        if (status === Loader.Error)
                            console.error("Failed to load Style for", root);
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        dialogWindow.width = Screen.width;
        dialogWindow.height = Screen.height;
    }
}
