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
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import Fluid.Ui 1.0 as FluidUi
import ".."

Item {
    readonly property alias panel: panel

    id: root

    Panel {
        id: panel
        z: 1
        onIndicatorTriggered: {
            // Set the last indicator
            rightDrawer.lastIndicator = indicator;
            indicator.active = true;

            // Load indicator component
            if (rightDrawer.expanded)
                loader.sourceComponent = indicator.component;
            else
                loader.sourceComponent = indicator.component;
            rightDrawer.open();
        }
    }

    Drawer {
        property var lastIndicator: null

        Material.theme: Material.Dark

        id: rightDrawer
        edge: Qt.RightEdge
        y: output.availableGeometry.y
        width: loader.implicitWidth + (2 * FluidUi.Units.largeSpacing)
        height: output.availableGeometry.height
        onPositionChanged: {
            if (position == 0.0 && lastIndicator)
                lastIndicator.active = false;
        }

        Pane {
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: loader.implicitWidth
            padding: FluidUi.Units.largeSpacing

            Loader {
                id: loader
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
            }
        }
    }
}
