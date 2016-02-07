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
import QtQuick.Controls 1.0
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaiios.hardware 0.1

Item {
    property var battery

    RowLayout {
        anchors {
            fill: parent
            margins: Themes.Units.largeSpacing
        }
        spacing: Themes.Units.smallSpacing

        Components.Icon {
            id: icon
            width: Themes.Units.iconSizes.large
            height: width
            color: Themes.Theme.palette.panel.textColor
            cache: false

            Layout.alignment: Qt.AlignTop
        }

        ColumnLayout {
            spacing: Themes.Units.smallSpacing

            Label {
                id: chargeStateLabel
                color: Themes.Theme.palette.panel.textColor

                Layout.fillWidth: true
            }

            RowLayout {
                spacing: Themes.Units.smallSpacing

                ProgressBar {
                    id: chargeProgress
                    minimumValue: 0
                    maximumValue: 100
                    value: battery ? battery.chargePercent : 0

                    Layout.fillWidth: true
                }

                Text {
                    renderType: Text.NativeRendering
                    text: (battery ? battery.chargePercent : 0) + "%"
                    color: Themes.Theme.palette.panel.textColor
                }

                Layout.fillWidth: true
            }

            GridLayout {
                rows: battery && battery.powerSupply ? 2 : 4
                columns: 2
                opacity: 0.6
                visible: battery !== null

                Text {
                    renderType: Text.NativeRendering
                    color: Themes.Theme.palette.panel.textColor
                    text: qsTr("Time To Empty")
                }

                Text {
                    renderType: Text.NativeRendering
                    color: Themes.Theme.palette.panel.textColor
                    // TODO: Convert to time
                    text: battery ? battery.timeToEmpty : 0
                    horizontalAlignment: Qt.AlignRight
                }

                Text {
                    renderType: Text.NativeRendering
                    color: Themes.Theme.palette.panel.textColor
                    text: qsTr("Capacity")
                }

                Text {
                    renderType: Text.NativeRendering
                    color: Themes.Theme.palette.panel.textColor
                    text: (battery ? battery.capacity : 0) + "%"
                    horizontalAlignment: Qt.AlignRight
                }

                Text {
                    renderType: Text.NativeRendering
                    color: Themes.Theme.palette.panel.textColor
                    text: qsTr("Vendor")
                    visible: battery && battery.powerSupply
                }

                Text {
                    renderType: Text.NativeRendering
                    color: Themes.Theme.palette.panel.textColor
                    text: battery ? battery.vendor : qsTr("n.a.")
                    horizontalAlignment: Qt.AlignRight
                    visible: battery && battery.powerSupply
                }

                Text {
                    renderType: Text.NativeRendering
                    color: Themes.Theme.palette.panel.textColor
                    text: qsTr("Model")
                    visible: battery && battery.powerSupply
                }

                Text {
                    renderType: Text.NativeRendering
                    color: Themes.Theme.palette.panel.textColor
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
            icon.iconName = "battery-empty-symbolic"
        else if (total < 20)
            icon.iconName = "battery-low-symbolic";
        else if (total < 40)
            icon.iconName = "battery-caution-symbolic";
        else if (total < 80)
            icon.iconName = "battery-good-symbolic";
        else
            icon.iconName = "battery-full-symbolic";
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
