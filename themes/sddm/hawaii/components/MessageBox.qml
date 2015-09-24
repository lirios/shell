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
import QtQuick.Layouts 1.0
import Hawaii.Themes 1.0 as Themes
import Hawaii.Components 1.0 as Components

RowLayout {
    property color color: Themes.Theme.palette.panel.textColor
    property alias iconName: icon.iconName
    property alias iconSource: icon.iconSource
    property alias message: label.text

    id: messageBox
    spacing: Themes.Units.smallSpacing
    opacity: 0.0

    Components.Icon {
        id: icon
        width: Themes.Units.iconSizes.small
        height: width
        color: messageBox.color
    }

    Text {
        id: label
        font.family: "Noto Sans"
        font.pointSize: Themes.Theme.defaultFont.pointSize * 1.2
        color: messageBox.color
        renderType: Text.NativeRendering
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: Themes.Units.shortDuration
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
