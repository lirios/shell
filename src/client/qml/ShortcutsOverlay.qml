/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Shell 1.0
import Hawaii.Shell.Styles 1.0

Item {
    width: Math.min(Math.max(mainLayout.implicitWidth, Screen.width * 0.5) + 2 * margin, Screen.width)
    height: Math.min(Math.max(mainLayout.implicitHeight, Screen.height * 0.5) + 2 * margin, Screen.height)
    opacity: active ? 1.0 : 0.0

    property bool active: false
    property int margin: 20

    Behavior on opacity {
        NumberAnimation { duration: 250 }
    }

    Shortcut {
        key: Qt.ControlModifier | Qt.AltModifier | Qt.Key_H
        onTriggered: active = !active
    }

    StyledItem {
        id: styledItem
        anchors.fill: parent
        style: Qt.createComponent(StyleSettings.path + "/OverlayStyle.qml", styledItem)

        RowLayout {
            id: mainLayout
            anchors {
                fill: parent
                margins: margin
            }
            spacing: margin

            ColumnLayout {
                spacing: margin

                Text {
                    text: "<h2>AppChooser</h2>" +
                          "<table border='0' cellspacing='10'>" +
                          "<tr><td><strong>Super + A</strong></td><td>Opens AppChooser, hitting this again closes it</td></tr>" +
                          "</table>"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.RichText
                }

                Rectangle {
                    color: styledItem.__style.textColor
                    height: 1

                    Layout.fillWidth: true
                }

                Text {
                    text: "<h2>Window Management</h2>" +
                          "<table border='0' cellspacing='10'>" +
                          "<tr><td><strong>Super + Tab</strong></td><td>Switch between windows in the current workspace</td></tr>" +
                          "<tr><td><strong>Super + E</strong></td><td>Expo mode for all windows in the current workspace</td></tr>" +
                          "</table>"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.RichText
                }

                Rectangle {
                    color: styledItem.__style.textColor
                    height: 1

                    Layout.fillWidth: true
                }

                Text {
                    text: "<h2>Workspace Management</h2>" +
                          "<table border='0' cellspacing='10'>" +
                          "<tr><td><strong>Super + G</strong></td><td>Zooms out on all the workspaces and<br>" +
                          "lets you manage windows (disabled)</td></tr>" +
                          "<tr><td><strong>Super + Left</strong></td><td>Go to previous workspace</td></tr>" +
                          "<tr><td><strong>Super + Right</strong></td><td>Go to next workspace</td></tr>" +
                          "</table>"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.RichText
                }

                Item {
                    Layout.fillHeight: true
                }
            }

            Rectangle {
                color: styledItem.__style.textColor
                width: 1

                Layout.fillHeight: true
            }

            ColumnLayout {
                spacing: margin

                Text {
                    text: "<h2>Screenshots</h2>" +
                          "<table border='0' cellspacing='10'>" +
                          "<tr><td><strong>Super + S</strong></td><td>Take a screenshot to the current workspace</td></tr>" +
                          "<tr><td><strong>Super + R</strong></td><td>Take a video of the current workspace</td></tr>" +
                          "</table>"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.RichText
                }

                Rectangle {
                    color: styledItem.__style.textColor
                    height: 1

                    Layout.fillWidth: true
                }

                Text {
                    text: "<h2>Miscellaneous</h2>" +
                          "<table border='0' cellspacing='10'>" +
                          "<tr><td><strong>Ctrl + Alt + H</strong></td><td>This screen</td></tr>" +
                          "<tr><td><strong>Ctrl + Alt + Backspace</strong></td><td>Terminate session immediately<br>(unsafe, use as last resort)</td></tr>" +
                          "<tr><td><strong>Super + Vertical Scroll</strong></td><td>Zoom in and out</td></tr>" +
                          "</table>"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.RichText
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }
}
