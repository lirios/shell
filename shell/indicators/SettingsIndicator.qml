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
import Hawaii.Controls 1.0 as Controls
import Fluid.Controls 1.0 as FluidUi
import ".."
import "../components" as CustomComponents

Indicator {
    name: "settings"
    iconName: "preferences-system-symbolic"
    component: Component {
        ColumnLayout {
            spacing: FluidUi.Units.largeSpacing

            Controls.Heading {
                text: qsTr("Settings")
            }

            Controls.Heading {
                text: qsTr("Size")
                level: 3
            }

            Row {
                spacing: FluidUi.Units.smallSpacing

                CustomComponents.ToolButton {
                    text: qsTr("Small")
                    checkable: true
                    checked: screenView.panel.size === FluidUi.Units.iconSizes.medium
                    autoExclusive: true
                    onClicked: screenView.panel.size = FluidUi.Units.iconSizes.medium
                }

                CustomComponents.ToolButton {
                    text: qsTr("Medium")
                    checkable: true
                    checked: screenView.panel.size === FluidUi.Units.iconSizes.large
                    autoExclusive: true
                    onClicked: screenView.panel.size = FluidUi.Units.iconSizes.large
                }

                CustomComponents.ToolButton {
                    text: qsTr("Large")
                    checkable: true
                    checked: screenView.panel.size === FluidUi.Units.iconSizes.huge
                    autoExclusive: true
                    onClicked: screenView.panel.size = FluidUi.Units.iconSizes.huge
                }
            }

            Controls.Heading {
                text: qsTr("Position")
                level: 3
            }

            Row {
                spacing: FluidUi.Units.smallSpacing

                CustomComponents.ToolButton {
                    text: qsTr("Left")
                    //iconName: "align-horizontal-left-symbolic"
                    checkable: true
                    checked: screenView.panel.state === "left"
                    autoExclusive: true
                    onClicked: screenView.panel.state = "left"
                }

                CustomComponents.ToolButton {
                    text: qsTr("Top")
                    //iconName: "align-vertical-top-symbolic"
                    checkable: true
                    checked: screenView.panel.state === "top"
                    autoExclusive: true
                    onClicked: screenView.panel.state = "top"
                }

                CustomComponents.ToolButton {
                    text: qsTr("Bottom")
                    //iconName: "align-vertical-bottom-symbolic"
                    checkable: true
                    checked: screenView.panel.state === "bottom"
                    autoExclusive: true
                    onClicked: screenView.panel.state = "bottom"
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
