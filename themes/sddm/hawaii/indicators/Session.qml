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

import QtQuick 2.1
import SddmComponents 2.0
import "../components" as Components

Components.Button {
    property alias currentIndex: listView.currentIndex

    iconName: "system-run-symbolic"
    text: listView.currentItem.sessionName
    //text: listView.model.get(listView.currentIndex).name
    visible: listView.count > 1
    onClicked: listView.incrementCurrentIndex()

    //: Session indicator accessibility name
    //~ Indicator to select a session
    Accessible.name: qsTr("Select session")

    ListView {
        id: listView
        model: sessionModel
        currentIndex: sessionModel.lastIndex
        highlightRangeMode: ListView.StrictlyEnforceRange
        orientation: ListView.Horizontal
        interactive: false
        clip: false
        keyNavigationWraps: true
        visible: false
        delegate: Item {
            property string sessionName: name
        }
    }
}
