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
import Hawaii.Shell 1.0
import Hawaii.Shell.Core 1.0
import Hawaii.Shell.Styles 1.0

Element {
    id: root

    QtObject {
        id: __priv

        property bool hover: false
        property bool checked: false
    }

    AppChooser {
        id: appChooser
    }

    Shortcut {
        key: Qt.MetaModifier | Qt.Key_A
        onTriggered: appChooser.visible = !appChooser.visible
    }

    StyledItem {
        id: styledItem
        anchors.fill: parent
        style: Qt.createComponent(StyleSettings.path + "/AppChooserButtonStyle.qml", styledItem)

        FluidUi.Icon {
            id: icon
            anchors.fill: parent
            iconName: "view-grid-symbolic"
            color: __priv.hover || __priv.checked ? styledItem.__style.highlightedColor : styledItem.__style.color

            Behavior on color {
                ColorAnimation { easing.type: Easing.Linear; duration: 250 }
            }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            onEntered: __priv.hover = true
            onExited: __priv.hover = __priv.checked
            onClicked: appChooser.visible = !appChooser.visible
        }
    }
}
