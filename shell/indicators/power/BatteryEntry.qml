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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import Fluid.UI 1.0 as FluidUi
import Hawaii.Hardware 1.0

Item {
    property var battery

    RowLayout {
        anchors {
            fill: parent
            margins: FluidUi.Units.largeSpacing
        }
        spacing: FluidUi.Units.smallSpacing

        FluidUi.Icon {
            id: icon
            width: FluidUi.Units.iconSizes.large
            height: width
            color: chargeStateLabel.color
            cache: false

            Layout.alignment: Qt.AlignTop
        }

        ColumnLayout {
            spacing: FluidUi.Units.smallSpacing

            Label {
                id: chargeStateLabel

                Layout.fillWidth: true
            }

            RowLayout {
                spacing: FluidUi.Units.smallSpacing

                ProgressBar {
                    id: chargeProgress
                    from: 0
                    to: 100
                    value: battery ? battery.chargePercent : 0

                    Layout.fillWidth: true
                }

                Label {
                    text: (battery ? battery.chargePercent : 0) + "%"
                }

                Layout.fillWidth: true
            }

            GridLayout {
                rows: battery && battery.powerSupply ? 2 : 4
                columns: 2
                opacity: 0.6
                visible: battery !== null

                Label {
                    text: qsTr("Time To Empty")
                }

                Label {
                    // TODO: Convert to time
                    text: battery ? battery.timeToEmpty : 0
                    horizontalAlignment: Qt.AlignRight
                }

                Label {
                    text: qsTr("Capacity")
                }

                Label {
                    text: (battery ? battery.capacity : 0) + "%"
                    horizontalAlignment: Qt.AlignRight
                }

                Label {
                    text: qsTr("Vendor")
                    visible: battery && battery.powerSupply
                }

                Label {
                    text: battery ? battery.vendor : qsTr("n.a.")
                    horizontalAlignment: Qt.AlignRight
                    visible: battery && battery.powerSupply
                }

                Label {
                    text: qsTr("Model")
                    visible: battery && battery.powerSupply
                }

                Label {
                    text: battery ? battery.product : qsTr("n.a.")
                    horizontalAlignment: Qt.AlignRight
                    visible: battery && battery.powerSupply
                }

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Layout.fillWidth: true
        }
    }

    Connections {
        target: battery
        onChargePercentChanged: setIconName()
        onChargeStateChanged: setChargeState()
    }

    function setIconName() {
        var total = battery.chargePercent;
        if (total < 5)
            icon.name = "battery-empty-symbolic"
        else if (total < 20)
            icon.name = "battery-low-symbolic";
        else if (total < 40)
            icon.name = "battery-caution-symbolic";
        else if (total < 80)
            icon.name = "battery-good-symbolic";
        else
            icon.name = "battery-full-symbolic";
    }

    function setChargeState() {
        var state = battery.chargeState;
        switch (state) {
        case Battery.Charging:
            chargeStateLabel.text = qsTr("Charging");
            break;
        case Battery.Discharging:
            chargeStateLabel.text = qsTr("Discharging");
            break;
        case Battery.FullyCharged:
            chargeStateLabel.text = qsTr("Fully charged");
            break;
        default:
            chargeStateLabel.text = qsTr("Stable");
            break;
        }
    }

    Component.onCompleted: {
        setIconName();
        setChargeState();
    }
}
