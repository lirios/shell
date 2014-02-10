/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Shell 1.0
import Hawaii.Shell.Styles 1.0

Item {
    id: panelView

    property Item containment

    onContainmentChanged: {
        containment.visible = true;
        containment.parent = styledItem;
        containment.anchors.fill = styledItem;
    }

    StyledItem {
        id: styledItem
        anchors.fill: parent
        style: Qt.createComponent(StyleSettings.path + "/PanelStyle.qml", panelView)
    }

    Item {
        id: configButton
        width: view.formFactor === Types.Horizontal ? parent.height : parent.width
        height: width
        visible: true
        states: [
            State {
                name: "horizontal"
                when: view.formFactor === Types.Horizontal

                AnchorChanges {
                    target: configButton
                    anchors.top: parent.top
                    anchors.right: parent.right
                }
            },
            State {
                name: "vertical"
                when: view.formFactor === Types.Vertical

                AnchorChanges {
                    target: configButton
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                }
                PropertyChanges {
                    target: configButton
                    anchors.leftMargin: (parent.width / 2) - (configButton.width / 2)
                }
            }
        ]

        MouseArea {
            anchors.fill: parent
            onClicked: view.configuring = !view.configuring;
        }

        FluidUi.Icon {
            anchors.fill: parent
            iconName: "preferences-system-windows"
        }

        Behavior on width {
            PropertyAnimation {}
        }

        Behavior on height {
            PropertyAnimation {}
        }
    }
}
