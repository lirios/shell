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

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0

RowLayout {
    property color color: Material.primaryTextColor
    property alias iconName: icon.name
    property alias iconSource: icon.iconSource
    property alias message: label.text

    id: messageBox
    spacing: Units.smallSpacing
    opacity: 0.0

    Icon {
        id: icon
        size: Units.iconSizes.small
        height: width
        color: messageBox.color
    }

    TitleLabel {
        id: label
        color: messageBox.color
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: Units.shortDuration
        }
    }

    Timer {
        interval: 4000
        running: messageBox.opacity == 1.0
        onTriggered: messageBox.opacity = 0.0
    }

    function setInfoMessage(msg) {
        messageBox.iconName = "dialog-information-symbolic";
        messageBox.message = msg;
        messageBox.color = "#00ff00";
        messageBox.opacity = 1.0;
    }

    function setErrorMessage(msg) {
        messageBox.iconName = "dialog-error-symbolic";
        messageBox.message = msg;
        messageBox.color = "#a63232";
        messageBox.opacity = 1.0;
    }
}
