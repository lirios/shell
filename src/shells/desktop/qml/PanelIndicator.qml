/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import Hawaii.Shell.Styles 0.1
import "PanelMenuManager.js" as PanelMenuManager

StyledItem {
    id: indicator

    // Label text and icon name
    property string label
    property string iconName

    // Menu object
    property PanelMenu menu

    // Icon size
    property real iconSize: 20 * __style.dpiScaleFactor

    // Selected and hovered
    property bool selected: false
    property bool hovered: false

    // Emitted when the indicator is clicked and doesn't have a menu
    signal clicked()

    style: Qt.createComponent("PanelIndicatorStyle.qml", indicator)

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            hovered = menu != null;

            if (menu && PanelMenuManager.triggered) {
                if (PanelMenuManager.currentIndicator) {
                    if (PanelMenuManager.currentIndicator.menu)
                        PanelMenuManager.currentIndicator.menu.close();

                    PanelMenuManager.currentIndicator.selected = false;
                }

                PanelMenuManager.currentIndicator = indicator;
                selected = true;
                menu.open();
            }
        }
        onExited: {
            hovered = false;
        }
        onClicked: {
            // If the indicator doesn't have a menu, it can handle the click
            if (!menu) {
                indicator.clicked();
                return;
            }

            selected = !selected;

            var pos = mapToItem(null, mouse.x, mouse.y);
            menu.x = Math.max(0, Math.min(pos.x - menu.width / 2, root.availableGeometry.width - menu.width));
            menu.y = root.availableGeometry.y;

            if (selected)
                menu.open();
            else
                menu.close();

            PanelMenuManager.triggered = selected;
            PanelMenuManager.currentIndicator = indicator;
        }
    }
}
