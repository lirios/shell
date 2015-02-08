/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2013-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.1
import QtQuick.Controls.Private 1.0 as QtControlsStylePrivate
import Hawaii.Themes 1.0

QtControlsStylePrivate.GroupBoxStyle {
    id: groupBoxStyle

    padding {
        top: (control.title.length > 0 || control.checkable ? Units.iconSizes.small : 0) + Units.largeSpacing
        left: Units.iconSizes.small
    }

    panel: Item {
        anchors.fill: parent
        Loader {
            id: checkboxloader
            anchors.left: parent.left
            sourceComponent: control.checkable ? checkbox : null
            anchors.verticalCenter: label.verticalCenter
            width: item ? item.implicitWidth : 0
        }

        Text {
            id: label
            anchors.top: parent.top
            anchors.left: checkboxloader.right
            anchors.margins: Units.smallSpacing
            text: control.title
            color: Theme.palette.panel.textColor
            font.bold: true
            renderType: Text.NativeRendering
        }
    }
}
