// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import Fluid.Controls

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
