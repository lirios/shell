/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

import QtQuick 2.1
import QtQuick.Controls 2.0 as QQC
import Fluid.Controls 1.0
import Hawaii.Session 1.0
import SddmComponents 2.0

Indicator {
    id: sessionIndicator

    property int currentIndex: sessionModel.lastIndex

    iconName: "action/settings_applications"
    visible: sessions.count > 1

    //: Session indicator tooltip
    //~ Indicator to select a session
    tooltip: qsTr("Select a session")

    //: Session indicator accessibility name
    //~ Indicator to select a session
    Accessible.name: qsTr("Select session")

    active: popup.visible
    onClicked: popup.open()

    // TODO: SDDM uses QQuickView not Application, so Drawer
    // doesn't work, let's use Popup for now even if it doesn't
    // show the modal background
    QQC.Popup {
        id: popup
        parent: greeter
        modal: true
        x: (greeter.width - width) / 2
        y: (greeter.height - height) / 2
        width: 250
        height: 250

        Column {
            anchors.fill: parent

            Repeater {
                id: sessions
                model: sessionModel

                ListItem {
                    text: name
                    highlighted: currentIndex == index
                    onClicked: currentIndex = index
                }
            }
        }
    }
}
