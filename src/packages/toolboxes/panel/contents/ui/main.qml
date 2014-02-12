/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Shell 1.0

Item {
    property bool isHorizontal: panel.formFactor === Types.Horizontal
    property int margin: 10

    id: root
    anchors {
        right: isHorizontal ? parent.right : undefined
        bottom: isHorizontal ? undefined : parent.bottom
        horizontalCenter: isHorizontal ? undefined : parent.horizontalCenter
        verticalCenter: isHorizontal ? parent.verticalCenter : undefined
    }
    width: button.width + (isHorizontal ? margin : 0)
    height: button.height + (isHorizontal ? 0 : margin)

    ToolButton {
        id: button
        iconName: "preferences-other-symbolic"
        onClicked: panel.configuring = !panel.configuring
    }
}
