/****************************************************************************
 * This file is part of Kahai.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Window 2.0
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import "indicators"

Window {
    id: shellRoot
    width: 1920
    height: 1080
    visible: true

    Image {
        id: background
        anchors.fill: parent
        source: "/usr/share/wallpapers/Fresh_Morning/contents/images/1920x1200.jpg"
    }

    Panel {
        id: panel
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        z: 1000

        onIndicatorTriggered: {
            // Close drawer if the current indicator is triggered again
            if (indicator.selected) {
                if (rightDrawer.status === PlasmaComponents.DialogStatus.Open) {
                    rightDrawer.close();
                    selectedIndicator = null;
                }

                return;
            }

            // Load indicator component
            stackView.push(indicator.component);

            // Open drawer if necessary
            if (rightDrawer.status === PlasmaComponents.DialogStatus.Closed)
                rightDrawer.open();

            // Save a reference to the currently open indicator
            selectedIndicator = indicator;
        }
    }

    SlidingPanel {
        id: leftDrawer
        edge: Qt.LeftEdge
        width: units.gridUnit * 20
    }

    SlidingPanel {
        id: rightDrawer
        edge: Qt.RightEdge
        width: units.gridUnit * 16

        StackView {
            id: stackView
            anchors.fill: parent
            anchors.margins: units.largeSpacing
        }
    }
}
