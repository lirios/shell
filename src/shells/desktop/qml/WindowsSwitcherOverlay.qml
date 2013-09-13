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
import Hawaii.Shell.Desktop 0.1
import Hawaii.Shell.Styles 0.1

Item {
    id: windowsSwitcher
    width: Math.min(mainLayout.implicitWidth, Screen.width) + 2 * margin
    height: Math.min(mainLayout.implicitHeight, Screen.height) + 2 * margin
    opacity: width > 0 ? 1.0 : 0.0

    readonly property int margin: 20
    readonly property int itemSize: 192
    readonly property int iconSize: 128

    Behavior on opacity {
        NumberAnimation { duration: 250 }
    }

    Behavior on width {
        NumberAnimation { duration: 100 }
    }

    Behavior on height {
        NumberAnimation { duration: 100 }
    }

    Connections {
        target: Shell
        onWindowActivated: {
            if (window.title)
                windowTitle.text = window.title;
            else if (window.appInfo && window.appInfo.name)
                windowTitle.text = window.appInfo.name;
            else
                windowTitle.text = qsTr("Untitled");
        }
        onWindowsChanged: {
            // Clear window title when there are no more windows
            if (Shell.windows.length == 0)
                windowTitle.text = "";
        }
        onWorkspaceSwitched: {
            opacity = 1.0;
            timer.start();
        }
    }

    Timer {
        id: timer
        repeat: false
        interval: 2000
        onTriggered: opacity = 0.0
    }

    StyledItem {
        id: styledItem
        anchors {
            fill: parent
            margins: margin
        }
        style: Qt.createComponent("OverlayStyle.qml", styledItem)

        ColumnLayout {
            id: mainLayout
            spacing: margin / 2

            Grid {
                rows: Math.min(Math.ceil(Shell.windows.length / columns), 5)
                columns: Math.min(Shell.windows.length, 7)
                spacing: margin / 2

                Repeater {
                    model: Shell.windows

                    Column {
                        Rectangle {
                            color: modelData.active
                                   ? Qt.lighter(styledItem.__style.panelColor, 2)
                                   : "transparent"
                            radius: 6
                            z: 0
                        }

                        Image {
                            source: {
                                var name = "application-x-executable";

                                if (modelData.appInfo && modelData.appInfo.iconName)
                                    name = modelData.appInfo.iconName;

                                return "image://appicon/" + name;
                            }
                            sourceSize.width: width
                            sourceSize.height: height
                            width: iconSize
                            height: iconSize
                            smooth: true
                            z: 1
                        }
                    }
                }
            }

            Text {
                id: windowTitle
                color: styledItem.__style.textColor
                style: Text.Raised
                styleColor: styledItem.__style.textShadowColor
                renderType: Text.NativeRendering
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                maximumLineCount: 2

                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.fillHeight: true
            }
        }
    }
}
