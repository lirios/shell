/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.2 as QtQuickControls
import QtQuick.Controls.Styles 1.2 as QtQuickControlsStyles
import Hawaii.Themes 1.0 as Themes
import Fluid.Controls 1.0 as FluidUi

QtQuickControls.Button {
    property string iconName
    property color color

    style: QtQuickControlsStyles.ButtonStyle {
        background: Item {}
        label: Row {
            spacing: Themes.Units.smallSpacing

            FluidUi.Icon {
                id: icon
                name: control.iconName
                color: control.color
                width: Themes.Units.iconSizes.smallMedium
                height: width
            }

            Text {
                id: label
                anchors.verticalCenter: icon.verticalCenter
                renderType: Text.NativeRendering
                text: control.text
                font.family: "Noto Sans"
                color: control.color
            }
        }
    }
}
