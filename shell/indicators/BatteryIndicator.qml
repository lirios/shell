/****************************************************************************
 * This file is part of Hawaii.
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

import QtQuick 2.0
import QtQuick.Layouts 1.0
import Fluid.Controls 1.0
import Hawaii.Desktop 1.0
import Hawaii.Hardware 1.0
import "power" as PowerIndicator

Indicator {
    name: "battery"
    iconName: deviceChargeIcon(hardwareEngine.primaryBattery)
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

    function deviceChargeIcon(device) {
        if (!device)
            return "device/battery_unknown"

        var level = "full"

        if (device.chargePercent < 25)
            level = "20"
        else if (device.chargePercent < 35)
            level = "30"
        else if (device.chargePercent < 55)
            level = "50"
        else if (device.chargePercent < 65)
            level = "60"
        else if (device.chargePercent < 85)
            level = "80"
        else if (device.chargePercent < 95)
            level = "90"

        if (device.chargeState == Battery.Charging ||
                device.chargeState == Battery.FullyCharged)
            return "device/battery_charging_" + level
        else
            return "device/battery_" + level
    }

    HardwareEngine {
        id: hardwareEngine
    }
}
