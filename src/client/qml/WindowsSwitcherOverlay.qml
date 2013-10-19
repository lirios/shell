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

import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Layouts 1.0
import Hawaii.Shell 0.2
import Hawaii.Shell.Styles 0.1

Item {
    id: windowsSwitcher
    width: Math.min(mainLayout.implicitWidth, Screen.width) + 2 * margin
    height: Math.min(mainLayout.implicitHeight, Screen.height) + 2 * margin
    opacity: 0.0

    readonly property int margin: 20
    readonly property int itemSize: iconSize + 2 * margin
    readonly property int iconSize: 128

    QtObject {
        id: privobj

        property int currentWindowIndex: -1
        property string windowTitle
    }

    Behavior on opacity {
        NumberAnimation { duration: 150 }
    }

    Behavior on width {
        NumberAnimation { duration: 100 }
    }

    Behavior on height {
        NumberAnimation { duration: 100 }
    }

    Connections {
        target: Shell
        onWindowSwitchingStarted: opacity = 1.0
        onWindowSwitchingFinished: opacity = 0.0
        onWindowSwitchingNext: {
            // Find current index
            for (var i = 0; i < Shell.windows.length; i++) {
                if (Shell.windows[i] === window) {
                    privobj.currentWindowIndex = i;
                    break;
                }
            }

            // Show current window title
            if (window.title)
                privobj.windowTitle = window.title;
            else if (window.appInfo && window.appInfo.name)
                privobj.windowTitle = window.appInfo.name;
            else
                privobj.windowTitle = qsTr("Untitled");
        }
        onWindowsChanged: {
            // Clear window title when there are no more windows
            if (Shell.windows.length == 0)
                privobj.windowTitle = "";
        }
    }

    StyledItem {
        id: styledItem
        anchors {
            fill: parent
            margins: margin
        }
        style: Qt.createComponent(StyleSettings.path + "/OverlayStyle.qml", styledItem)

        ColumnLayout {
            id: mainLayout
            spacing: margin

            GridView {
                readonly property int rows: Math.min(Math.ceil(Shell.windows.length / columns), 5)
                readonly property int columns: Math.min(Shell.windows.length, 7)

                id: grid
                cellWidth: itemSize
                cellHeight: itemSize
                model: Shell.windows
                currentIndex: privobj.currentWindowIndex
                delegate: Item {
                    width: itemSize
                    height: itemSize

                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        source: "image://appicon/" + model.appInfo.iconName
                        sourceSize.width: width
                        sourceSize.height: height
                        width: iconSize
                        height: iconSize
                        smooth: true
                    }
                }
                highlight: Item {
                    width: itemSize
                    height: itemSize

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: margin / 2
                        color: Qt.lighter(styledItem.__style.panelColor1, 2)
                        radius: 6
                    }
                }

                Layout.minimumWidth: columns * cellWidth
                Layout.minimumHeight: rows * cellHeight
                Layout.maximumWidth: columns * cellWidth
                Layout.maximumHeight: rows * cellHeight
            }

            Text {
                text: privobj.windowTitle
                color: styledItem.__style.textColor
                style: Text.Raised
                styleColor: styledItem.__style.textShadowColor
                renderType: Text.NativeRendering
                horizontalAlignment: Qt.AlignHCenter
                font.bold: true
                elide: Text.ElideRight

                Layout.preferredWidth: grid.columns * grid.cellWidth - 2 * margin
                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            }
        }
    }
}
