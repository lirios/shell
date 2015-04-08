/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.1
import Hawaii.Themes 1.0 as Themes
import ".."
import "../components"

Item {
    readonly property alias panel: panel

    signal panelHeightChanged()

    id: root

    Panel {
        id: panel
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        z: 1
        onHeightChanged: root.panelHeightChanged()
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
                stackView.push({item: indicator.component, immediate:!rightDrawer.expanded});

            // Open drawer if necessary
            if (!rightDrawer.expanded)
                rightDrawer.open();

            // Save a reference to the currently open indicator
            selectedIndicator = indicator;
            lastIndicator = indicator;
        }
    }

    SlidingPanel {
        property alias component: loader.sourceComponent

        id: leftDrawer
        edge: Qt.LeftEdge
        width: Themes.Units.gu(20)
        z: 0

        Loader {
            id: loader
            anchors.fill: parent
            anchors.margins: Themes.Units.largeSpacing
            active: leftDrawer.expanded
        }
    }

    SlidingPanel {
        id: rightDrawer
        edge: Qt.RightEdge
        width: Themes.Units.gu(16)
        z: 0

        StackView {
            id: stackView
            anchors.fill: parent
            anchors.margins: Themes.Units.largeSpacing
        }
    }

    Component.onCompleted: setAvailableGeometry(panel.height)
}
