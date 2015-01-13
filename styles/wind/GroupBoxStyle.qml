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

import QtQuick 2.1
import QtQuick.Controls.Styles 1.1 as QtControlsStyle
import QtQuick.Controls.Private 1.0 as QtControlsPrivate
import Hawaii.Themes 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import "private/Utils.js" as Utils

QtControlsPrivate.GroupBoxStyle {
    id: groupBoxStyle

    padding {
        left: units.smallSpacing * 2
        top: (units.smallSpacing * 2.5) + Theme.mSize(label.font).height
        right: units.smallSpacing * 2
        bottom: units.smallSpacing * 2
    }

    property Component label: PlasmaComponents.Label {
        text: control.title
        font.weight: Font.Bold
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

        Rectangle {
            anchors.fill: parent
            anchors.topMargin: Theme.mSize(label.font).height
            border.color: Utils.blendColors(Theme.palette.panel.backgroundColor, Theme.palette.panel.textColor, 0.2)
            border.width: 1
            color: Utils.blendColors(Theme.palette.panel.backgroundColor, Theme.palette.view.backgroundColor, 0.5)
            radius: units.gridUnit * 0.5
            visible: !control.flat
        }

        Loader {
            id: labelLoader
            anchors.top: parent.top
            anchors.left: checkboxloader.right
            anchors.topMargin: -units.smallSpacing * 1.2
            sourceComponent: label
        }
    }
}
