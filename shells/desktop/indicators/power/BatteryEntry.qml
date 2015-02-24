/****************************************************************************
 * This file is part of Hawaii.
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
import Hawaii.Themes 1.0 as Themes
import org.hawaii.hardware 0.1

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
        }

        ColumnLayout {
            spacing: Themes.Units.smallSpacing

            Label {
                id: chargeStateLabel
                color: Themes.Theme.palette.panel.textColor

                Layout.fillWidth: true
            }

            ProgressBar {
                id: chargeProgress
                minimumValue: 0
                maximumValue: 100
                value: battery ? battery.chargePercent : 0

                Layout.fillWidth: true
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
