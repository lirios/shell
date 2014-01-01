/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Shell.Styles 1.0

StyledItem {
    id: root

    property bool hover: false
    property bool checked: false

    signal clicked()

    style: Qt.createComponent(StyleSettings.path + "/AppChooserButtonStyle.qml", root)

    FluidUi.Icon {
        id: icon
        anchors.fill: parent
        iconName: "view-grid-symbolic"
        color: hover || checked ? __style.highlightedColor : __style.color

        Behavior on color {
            ColorAnimation { easing.type: Easing.Linear; duration: 250 }
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onEntered: hover = true
        onExited: hover = checked
        onClicked: root.clicked()
    }
}
