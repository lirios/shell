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
import Hawaii.Shell 1.0
import SddmComponents 2.0

Indicator {
    id: keyboardIndicator

    iconName: "hardware/keyboard"
    visible: keyboard.layouts.length > 1

    //: Keyboard layout indicator tooltip
    //~ Indicator to change keyboard layout
    tooltip: qsTr("Change keyboard layout")

    //: Keyboard layout indicator accessibility name
    //~ Indicator to change keyboard layout
    Accessible.name: qsTr("Switch layout")

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
                model: keyboard.layouts

                ListItem {
                    text: modelData.longName
                    highlighted: index === keyboard.currentLayout
                    onClicked: keyboard.currentLayout = index
                }
            }
        }
    }
}
