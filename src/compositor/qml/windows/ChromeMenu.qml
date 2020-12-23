/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.15
import QtQuick.Controls 2.15
import Fluid.Controls 1.0

Menu {
    focus: false

    MenuItem {
        focusPolicy: Qt.NoFocus
        text: qsTr("Minimize")
        onClicked: chrome.window.minimized = true
    }

    MenuItem {
        focusPolicy: Qt.NoFocus
        text: qsTr("Restore")
        height: chrome.window.maximized ? implicitHeight : 0
        onClicked: chrome.window.sendUnmaximized()
    }

    MenuItem {
        focusPolicy: Qt.NoFocus
        text: qsTr("Maximize")
        height: chrome.window.maximized ? 0 : implicitHeight
        onClicked: chrome.window.sendMaximized(shellSurfaceItem.output)
    }

    MenuSeparator {}

    MenuItem {
        focusPolicy: Qt.NoFocus
        text: qsTr("Move")
    }

    MenuItem {
        focusPolicy: Qt.NoFocus
        text: qsTr("Resize")
    }

    MenuSeparator {}

    MenuItem {
        text: qsTr("Close")
        onClicked: chrome.window.sendClose()
    }
}
