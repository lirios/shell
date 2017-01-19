/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.4
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import Fluid.Controls 1.0
import Liri.Shell 1.0

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
