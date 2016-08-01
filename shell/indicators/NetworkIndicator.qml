/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import Fluid.Controls 1.0
import Hawaii.NetworkManager 1.0 as NM
import ".."
import "network" as NetworkIndicator

Indicator {
    id: indicator
    name: "network"
    iconName: massageIconName(connectionIconProvider.connectionIcon)
    component: Component {
        ColumnLayout {
            spacing: Units.largeSpacing

            HeadlineLabel {
                text: qsTr("Network")
            }

            NetworkIndicator.AirplaneMode {
                onClicked: handler.enableAirplaneMode(airplaneMode)
            }

            ListView {
                model: NM.AppletProxyModel {
                    sourceModel: NM.NetworkModel {}
                }
                clip: true
                currentIndex: -1
                section.property: "Section"
                section.delegate: NetworkIndicator.Header { text: section }
                delegate: NetworkIndicator.ConnectionItem {}

                ScrollBar.horizontal: ScrollBar {}
                ScrollBar.vertical: ScrollBar {}

                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
    visible: connectionIconProvider.connectionIcon !== "network-unavailable"

    NM.ConnectionIcon {
        id: connectionIconProvider
    }

    NM.Handler {
        id: handler
    }

    function massageIconName(iconName) {
        var newName = iconName.replace("-activated", "");
        if (newName !== "")
            return newName + "-symbolic";
        return newName;
    }
}
