/****************************************************************************
 * This file is part of Kahai.
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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import "../.."

RowLayout {
    property alias text: label.text

    spacing: units.smallSpacing
    width: ListView.view.width
    height: Math.max(label.paintedHeight, refreshButton.height) + (units.smallSpacing * 2)

    Label {
        id: label
        font.weight: Font.DemiBold
        color: Theme.panel.textColor

        Layout.fillWidth: true
    }

    ToolButton {
        id: refreshButton
        iconName: "refresh-symbolic"
    }
}
