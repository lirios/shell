/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Controls 1.0
import "../panel"

Item {
    id: shell

    readonly property alias panel: panel

    Panel {
        id: panel

        z: 1

        onIndicatorTriggered: {
            console.log('Triggered!')
            // Set the last indicator
            rightDrawer.lastIndicator = indicator;
            indicator.active = true;

            // Load indicator component
            if (rightDrawer.expanded)
                stackView.push(indicator.component, {}, StackView.Transition);
            else
                stackView.push(indicator.component, {}, StackView.Immedite);
            rightDrawer.open();
        }
    }

    Drawer {
        property var lastIndicator: null

        Material.theme: Material.Dark

        id: rightDrawer
        edge: Qt.RightEdge

        y: output.availableGeometry.y
        width: Math.max(250, stackView.currentItem.implicitWidth) + (2 * padding)
        height: output.availableGeometry.height
        onPositionChanged: {
            if (position == 0.0 && lastIndicator)
                lastIndicator.active = false;
        }

        Pane {
            anchors.fill: parent
            padding: Units.largeSpacing

            StackView {
                id: stackView
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
            }
        }
    }
}
