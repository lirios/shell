/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls.Styles 1.1 as QtControlsStyle
import Hawaii.Components 1.0 as Components
import QtQuick.Controls.Styles.Wind 1.0 as Wind
import org.kde.plasma.core 2.0 as PlasmaCore
import "private/Utils.js" as Utils

QtControlsStyle.TextFieldStyle {
    property QtObject clearButton: QtObject {
        property string iconName: "edit-clear-rtl-symbolic"
        property url iconSource: ""
        property color color: Wind.Palette.view.textColor
        property real iconSize: units.iconSizes.small
    }

    id: style
    textColor: Wind.Palette.view.textColor
    selectionColor: Wind.Palette.view.selectedBackgroundColor
    selectedTextColor: Wind.Palette.view.selectedTextColor
    placeholderTextColor: Utils.rgba(Wind.Palette.view.textColor, 0.5)
    background: Item {
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            anchors.bottomMargin: -1
            color: Utils.rgba(Wind.Palette.view.backgroundColor, 0.26)
            radius: baserect.radius
        }

        Rectangle {
            id: baserect
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { color: Qt.darker(Wind.Palette.view.backgroundColor, 1.2); position: 0}
                GradientStop { color: Wind.Palette.view.backgroundColor; position: 0.1 }
                GradientStop { color: Wind.Palette.view.backgroundColor; position: 1 }
            }
            radius: 4
            border.color: control.activeFocus ? Wind.Palette.view.selectedBackgroundColor : Qt.darker(Wind.Palette.view.backgroundColor, 1.5)
        }
    }
}
