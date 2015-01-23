/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.0
import Hawaii.Themes 1.0 as Themes

PopupBase {
    property alias text: label.text

    id: root
    popup: Rectangle {
        color: Themes.Theme.palette.panel.backgroundColor
        radius: Themes.Units.dp(6)
        width: label.width + (Themes.Units.largeSpacing * 2)
        height: label.height + (Themes.Units.smallSpacing * 2)
        border.color: Themes.Theme.palette.rgba(Qt.darker(Themes.Theme.palette.panel.backgroundColor, 1.5), 0.25)
        border.width: Themes.Units.dp(1)
        opacity: 0.0
        onOpacityChanged: {
            if (opacity == 0.0)
                root.closed();
        }

        Label {
            id: label
            anchors.centerIn: parent
            color: Themes.Theme.palette.panel.textColor
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            style: Text.Raised
            styleColor: Themes.Theme.palette.panel.textEffectColor
            horizontalAlignment: Text.AlignHCenter
            width: Math.min(implicitWidth, Themes.Units.dp(100))
            height: implicitHeight
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.longDuration
            }
        }
    }
}
