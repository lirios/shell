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

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.3
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes
import ".."

Indicator {
    name: "settings"
    iconName: "preferences-system-symbolic"
    component: Component {
        ColumnLayout {
            spacing: Themes.Units.largeSpacing

            Controls.Heading {
                text: qsTr("Settings")
                color: Themes.Theme.palette.panel.textColor
            }

            Controls.Heading {
                text: qsTr("Size")
                color: Themes.Theme.palette.panel.textColor
                level: 3
            }

            Row {
                spacing: Themes.Units.smallSpacing

                ExclusiveGroup {
                    id: sizeGroup
                }

                ToolButton {
                    text: qsTr("Small")
                    checkable: true
                    checked: screenView.panel.size === Themes.Units.iconSizes.medium
                    exclusiveGroup: sizeGroup
                    onClicked: screenView.panel.size = Themes.Units.iconSizes.medium
                }

                ToolButton {
                    text: qsTr("Medium")
                    checkable: true
                    checked: screenView.panel.size === Themes.Units.iconSizes.large
                    exclusiveGroup: sizeGroup
                    onClicked: screenView.panel.size = Themes.Units.iconSizes.large
                }

                ToolButton {
                    text: qsTr("Large")
                    checkable: true
                    checked: screenView.panel.size === Themes.Units.iconSizes.huge
                    exclusiveGroup: sizeGroup
                    onClicked: screenView.panel.size = Themes.Units.iconSizes.huge
                }
            }

            /*
            Controls.Heading {
                text: qsTr("Position")
                color: Themes.Theme.palette.panel.textColor
                level: 3
            }

            Row {
                spacing: Themes.Units.smallSpacing

                ExclusiveGroup {
                    id: posGroup
                }

                ToolButton {
                    text: qsTr("Left")
                    iconName: "align-horizontal-left-symbolic"
                    checkable: true
                    checked: screenView.panel.state === "vleft"
                    exclusiveGroup: posGroup
                    onClicked: screenView.panel.state = "vleft"
                }

                ToolButton {
                    text: qsTr("Top")
                    iconName: "align-horizontal-top-symbolic"
                    checkable: true
                    checked: screenView.panel.state === "htop"
                    exclusiveGroup: posGroup
                    onClicked: screenView.panel.state = "htop"
                }

                ToolButton {
                    text: qsTr("Right")
                    iconName: "align-horizontal-right-symbolic"
                    checkable: true
                    checked: screenView.panel.state === "vright"
                    exclusiveGroup: posGroup
                    onClicked: screenView.panel.state = "vright"
                }

                ToolButton {
                    text: qsTr("Bottom")
                    iconName: "align-horizontal-bottom-symbolic"
                    checkable: true
                    checked: screenView.panel.state === "hbottom"
                    exclusiveGroup: posGroup
                    onClicked: screenView.panel.state = "hbottom"
                }
            }
            */

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
