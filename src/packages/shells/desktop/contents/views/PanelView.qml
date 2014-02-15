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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
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
        buttonContainer.parent = containment;
        containment.configButton = buttonContainer;
    }

    QtObject {
        id: d

        property bool isHorizontal: panel.formFactor === Types.Horizontal
        property int margin: 5
    }

    StyledItem {
        id: styledItem
        anchors.fill: parent
        style: Qt.createComponent(StyleSettings.path + "/PanelStyle.qml", panelView)
    }

    Item {
        id: buttonContainer
        anchors {
            right: d.isHorizontal ? parent.right : undefined
            bottom: d.isHorizontal ? undefined : parent.bottom
            horizontalCenter: d.isHorizontal ? undefined : parent.horizontalCenter
            verticalCenter: d.isHorizontal ? parent.verticalCenter : undefined
        }
        width: button.width + (d.isHorizontal ? d.margin * 2 : 0)
        height: button.height + (d.isHorizontal ? 0 : d.margin * 2)
        z: 2000

        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: button
                iconName: "preferences-other-symbolic"
                onClicked: panel.configuring = !panel.configuring

                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }
    }
}
