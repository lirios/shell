/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import Hawaii.Components 1.0 as Components
import org.kde.plasma.core 2.0 as PlasmaCore
import "../.."

RowLayout {
    readonly property alias airplaneMode: __priv.airplaneMode

    signal clicked()

    id: root
    spacing: units.smallSpacing

    QtObject {
        id: __priv

        property bool airplaneMode: false
    }

    Components.Icon {
        id: airplaneModeIcon
        iconName: airplaneMode ? "airplane-mode-symbolic" : "airplane-mode-disabled-symbolic"
        width: units.iconSizes.smallMedium
        height: width
        color: Theme.panel.textColor
    }

    Label {
        text: airplaneMode ? qsTr("Airplane mode enabled") : qsTr("Airplane mode disabled")
        color: Theme.panel.textColor
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            __priv.airplaneMode = !__priv.airplaneMode;
            root.clicked();
        }
    }
}
