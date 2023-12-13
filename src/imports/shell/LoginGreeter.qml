// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Fluid.Controls
import Liri.Shell

ListView {
    id: usersListView

    property size itemSize: Qt.size(200, 220)
    property size itemSelectedSize: Qt.size(230, 280)

    signal loginRequested(string userName, string password)
    signal loginSucceeded()
    signal loginFailed(string message)

    ScrollIndicator.horizontal: ScrollIndicator {}

    width: count > 1 ? itemSelectedSize.width + ((itemSize.width + spacing) * Math.min(count - 1, 3)) : itemSelectedSize.width + (spacing * 2)
    height: itemSelectedSize.height + spacing
    orientation: ListView.Horizontal
    spacing: Units.smallSpacing
    keyNavigationWraps: true
    focus: true
    clip: true
    highlightMoveVelocity: 800
    currentIndex: -1
    delegate: UserDelegate {
        id: userDelegate
        onAccepted: usersListView.loginRequested(userName, field.text)
    }

    Component.onCompleted: currentIndex = 0
}
