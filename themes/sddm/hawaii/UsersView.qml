/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import Hawaii.Themes 1.0 as Themes

ListView {
    readonly property int userItemWidth: Themes.Units.largeSpacing * 8
    readonly property int userItemHeight: Themes.Units.largeSpacing * 8
    readonly property int userFaceSize: Themes.Units.largeSpacing * 6

    signal selected()

    id: usersView
    focus: true
    spacing: Themes.Units.largeSpacing * 2
    orientation: ListView.Horizontal
    highlightRangeMode: ListView.StrictlyEnforceRange
    delegate: UserDelegate {
        name: (model.realName === "") ? model.name : model.realName
        userName: model.name
        iconSource: model.icon
        faceSize: usersView.userFaceSize
        width: usersView.userItemWidth
        height: usersView.userItemHeight
        onClicked: ListView.view.currentIndex = index
    }
    cacheBuffer: 100
    currentIndex: usersView.model && usersView.model.lastIndex ? usersView.model.lastIndex : 0
    onCurrentIndexChanged: usersView.selected()

    Keys.onEscapePressed: usersView.selected()
    Keys.onEnterPressed: usersView.selected()
    Keys.onReturnPressed: usersView.selected()
}
