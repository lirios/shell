/****************************************************************************
 * This file is part of Kahai.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import Hawaii.Components 1.0 as Components
import ".."
import "network" as NetworkIndicator

Indicator {
    id: indicator
    name: "network"
    iconName: massageIconName(connectionIconProvider.connectionIcon)
    component: Component {
        ColumnLayout {
            spacing: units.largeSpacing

            PlasmaExtras.Heading {
                text: qsTr("Network")
                color: Theme.panel.textColor
            }

            Components.Icon {
                property bool airplaneMode: false

                iconName: airplaneMode ? "airplane-mode-symbolic" : "airplane-mode-disabled-symbolic"
                width: units.iconSizes.smallMedium
                height: width
                color: Theme.panel.textColor

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        handler.enableAirplaneMode(airplaneMode);
                        airplaneMode = !airplaneMode;
                    }
                }
            }

            ScrollView {
                ListView {
                    model: PlasmaNM.AppletProxyModel {
                        sourceModel: PlasmaNM.NetworkModel {}
                    }
                    clip: true
                    currentIndex: -1
                    section.property: "Section"
                    section.delegate: NetworkIndicator.Header { text: section }
                    delegate: NetworkIndicator.ConnectionItem {}
                }

                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
    visible: connectionIconProvider.connectionIcon !== "network-unavailable"

    PlasmaNM.ConnectionIcon {
        id: connectionIconProvider
    }

    PlasmaNM.Handler {
        id: handler
    }

    function massageIconName(iconName) {
        var newName = iconName.replace("-activated", "");
        if (newName !== "")
            return newName + "-symbolic";
        return newName;
    }
}
