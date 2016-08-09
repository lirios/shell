/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
import QtQuick.Controls 2.0
import Fluid.Controls 1.0

RowLayout {
    readonly property alias airplaneMode: __priv.airplaneMode

    signal clicked()

    id: root
    spacing: Units.smallSpacing

    QtObject {
        id: __priv

        property bool airplaneMode: false
    }

    Switch {
        id: airplaneSwitch
        onClicked: {
            __priv.airplaneMode = !__priv.airplaneMode;
            root.clicked();
        }
    }

    Icon {
        id: airplaneModeIcon
        name: "airplane-mode-symbolic"
        width: Units.iconSizes.smallMedium
        height: width
        color: label.color
    }

    Label {
        id: label
        text: airplaneMode ? qsTr("Airplane mode enabled") : qsTr("Airplane mode disabled")
    }
}
