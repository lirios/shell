/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Shell.Desktop 0.1
import Hawaii.Shell.Styles 0.1

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
        style: Qt.createComponent("OverlayStyle.qml", styledItem)

        ColumnLayout {
            id: mainLayout
            anchors {
                fill: parent
                margins: margin
            }

            ColumnLayout {
                Text {
                    text: "<h2>AppChooser</h2><br>" +
                          "<p><strong>Super + A</strong> - Opens AppChooser, hitting this again closes it</p>"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.StyledText
                }

                Text {
                    text: "<h2>Window Management</h2><br>" +
                          "<p><strong>Alt + Tab</strong> - Switch between windows in the current workspace</p>" +
                          "<p><strong>Super + E</strong> - Expo mode for all windows in the current workspace</p>"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.StyledText
                }

                Text {
                    text: "<h2>Workspace Management</h2><br>" +
                          "<p><strong>Super + G</strong> - Zooms out on all the workspaces and lets you manage windows (incomplete feature)" +
                          "<p><strong>Super + Left</strong> - Go to previous workspace" +
                          "<p><strong>Super + Right</strong> - Go to next workspace"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.StyledText
                }

                Text {
                    text: "<h2>Screenshots</h2><br>" +
                          "<p><strong>Super + S</strong> - Take a screenshot to the current workspace</p>"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.StyledText
                }

                Text {
                    text: "<h2>Miscellaneous</h2><br>" +
                          "<p><strong>Ctrl + Alt + H</strong> - This screen</p>" +
                          "<p><strong>Ctrl + Alt + Backspace</strong> - Terminate session immediately (unsafe, use as last resort)</p>"
                    color: styledItem.__style.textColor
                    style: Text.Raised
                    styleColor: styledItem.__style.textShadowColor
                    textFormat: Text.StyledText
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
