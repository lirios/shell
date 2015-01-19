/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import Hawaii.Components 1.0 as Components
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes
import ".."
import "network" as NetworkIndicator

Indicator {
    id: indicator
    name: "network"
    iconName: massageIconName(connectionIconProvider.connectionIcon)
    component: Component {
        ColumnLayout {
            spacing: Themes.Units.largeSpacing

            Controls.Heading {
                text: qsTr("Network")
                color: Themes.Theme.palette.panel.textColor
            }

            NetworkIndicator.AirplaneMode {
                onClicked: handler.enableAirplaneMode(airplaneMode)
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
