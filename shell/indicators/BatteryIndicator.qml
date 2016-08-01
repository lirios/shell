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

import QtQuick 2.0
import QtQuick.Layouts 1.0
import Fluid.Controls 1.0
import Hawaii.Hardware 1.0
import ".."
import "power" as PowerIndicator

Indicator {
    name: "battery"
    iconName: "battery-missing-symbolic"
    component: Component {
        ColumnLayout {
            spacing: Units.largeSpacing

            HeadlineLabel {
                text: qsTr("Power")
            }

            Repeater {
                model: hardwareEngine.batteries

                PowerIndicator.BatteryEntry {
                    battery: modelData

                    Layout.fillWidth: true
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
    visible: hardwareEngine.batteries.length > 0

    HardwareEngine {
        id: hardwareEngine
        onBatteriesChanged: determineIconName()
    }

    function determineIconName() {
        var i, total = 0, battery = null;
        var charging = false, fullyCharged = false;
        for (i = 0; i < hardwareEngine.batteries.length; i++) {
            battery = hardwareEngine.batteries[i];
            total = Math.max(battery.chargePercent, total);

            if (battery.chargeState == Battery.Charging)
                charging = true;
            fullyCharged = battery.chargeState == Battery.FullyCharged;
        }

        if (total < 5)
            iconName = "battery-empty-" + (charging ? "charging-" : "") + "symbolic"
        else if (total < 20)
            iconName = "battery-low-" + (charging ? "charging-" : "") + "symbolic";
        else if (total < 40)
            iconName = "battery-caution-" + (charging ? "charging-" : "") + "symbolic";
        else if (total < 80)
            iconName = "battery-good-" + (charging ? "charging-" : "") + "symbolic";
        else
            iconName = "battery-full-" + (fullyCharged ? "charged-" : (charging ? "charging-" : "")) + "symbolic";
    }

    Component.onCompleted: determineIconName()
}
