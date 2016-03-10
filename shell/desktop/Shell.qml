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
import Qt.labs.controls 1.0 as LabsControls
import Fluid.Ui 1.0 as FluidUi
import ".."
import "../components" as CustomComponents

Item {
    readonly property alias panel: panel

    id: root

    Panel {
        id: panel
        z: 1
        onIndicatorTriggered: {
            // Close drawer if the current indicator is triggered again
            if (indicator.selected) {
                if (rightDrawer.expanded) {
                    rightDrawer.close();
                    selectedIndicator = null;
                }

                return;
            }

            // Load indicator component
            if (indicator !== lastIndicator)
                stackView.push(indicator.component, {}, rightDrawer.expanded ? LabsControls.StackView.Transition : LabsControls.StackView.Immedite);

            // Open drawer if necessary
            if (!rightDrawer.expanded)
                rightDrawer.open();

            // Save a reference to the currently open indicator
            selectedIndicator = indicator;
            lastIndicator = indicator;
        }
    }

    LabsControls.Drawer {
        id: rightDrawer
        edge: Qt.RightEdge
        //z: 0
        onClicked: close()

        //CustomComponents.Pane {
        LabsControls.Pane {
            width: FluidUi.Units.gu(16)
            height: output.availableGeometry.height
            padding: FluidUi.Units.largeSpacing

            LabsControls.StackView {
                id: stackView
                anchors.fill: parent
            }
        }
    }
}
