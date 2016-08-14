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
import QtQuick.Controls 2.0
import Fluid.Controls 1.0
import SddmComponents 2.0

IconButton {
    iconName: "input-keyboard-symbolic"
    text: keyboard.layouts[keyboard.currentLayout].shortName
    visible: keyboard.layouts.length > 1
    onClicked: {
        var index = keyboard.currentLayout;
        if (index == (keyboard.layouts.length - 1))
            index = -1;
        keyboard.currentLayout = index + 1;
    }

    //: Keyboard layout indicator tooltip
    //~ Indicator to change keyboard layout
    ToolTip.text: qsTr("Change keyboard layout")
    ToolTip.visible: hovered

    //: Keyboard layout indicator accessibility name
    //~ Indicator to change keyboard layout
    Accessible.name: qsTr("Switch layout")
}
