/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import Fluid.Controls 1.0
import Liri.Shell 1.0
import Vibe.NetworkManager 1.0 as NM
import "network" as NetworkIndicator

Indicator {
    id: indicator

    title: qsTr("Network")
    iconName: materialIconName(connectionIconProvider.connectionTooltipIcon)
    tooltip: networkStatus.activeConnections
    // visible: connectionIconProvider.connectionIcon !== "network-unavailable"

    component: ColumnLayout {
        spacing: 0

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
            section.delegate: Subheader { text: section }

            delegate: NetworkIndicator.ConnectionItem {}

            ScrollBar.horizontal: ScrollBar {}
            ScrollBar.vertical: ScrollBar {}

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    NM.ConnectionIcon {
        id: connectionIconProvider
    }

    NM.NetworkStatus {
        id: networkStatus
    }

    NM.Handler {
        id: handler
    }

    function materialIconName(iconName) {
        if (icons[iconName]) {
            return icons[iconName]
        } else {
            console.error("Icon not mapped: " + iconName)
        }
    }

    readonly property var icons: {
        "": "device/signal_wifi_0_bar",
        "network-wireless-100-locked": "device/signal_wifi_4_bar_lock",
        "network-wireless-80-locked": "device/signal_wifi_3_bar_lock",
        "network-wireless-60-locked": "device/signal_wifi_2_bar_lock",
        "network-wireless-40-locked": "device/signal_wifi_2_bar_lock",
        "network-wireless-20-locked": "device/signal_wifi_1_bar_lock",
        "network-wireless-0-locked": "device/signal_wifi_0_bar_lock",
        "network-wireless-connected-100": "device/signal_wifi_4_bar",
        "network-wireless-connected-80": "device/signal_wifi_3_bar",
        "network-wireless-connected-60": "device/signal_wifi_2_bar",
        "network-wireless-connected-40": "device/signal_wifi_2_bar",
        "network-wireless-connected-20": "device/signal_wifi_1_bar",
        "network-wireless-connected-0": "device/signal_wifi_0_bar",
        "network-wireless-100": "device/signal_wifi_4_bar",
        "network-wireless-60": "device/signal_wifi_2_bar",
        "network-wireless-40": "device/signal_wifi_2_bar",
        "network-wireless-80": "device/signal_wifi_3_bar",
        "network-wireless-20": "device/signal_wifi_1_bar",
        "network-wireless-0": "device/signal_wifi_0_bar",
        "network-wired-activated": "action/settings_ethernet",
        "network-wired": "action/settings_ethernet",
        "network-vpn": "action/lock",
        "network-bluetooth": "device/bluetooth",
        "network-unavailable": "device/signal_wifi_off",
    }
}
