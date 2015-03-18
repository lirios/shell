/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtGraphicalEffects 1.0
import GreenIsland 1.0
import Hawaii.Themes 1.0 as Themes
import org.hawaii.misc 0.1 as Misc
import ".."
import "../components"
import "../indicators"

Item {
    readonly property string name: _greenisland_output.name
    readonly property int number: _greenisland_output.number
    readonly property bool primary: _greenisland_output.primary
    property alias showInformation: outputInfo.visible

    readonly property alias workspacesView: workspacesLayer
    readonly property alias currentWorkspace: workspacesLayer.currentWorkspace
    property alias zoomEnabled: zoomArea.enabled

    property var layers: QtObject {
        readonly property alias workspaces: workspacesLayer
        readonly property alias fullScreen: fullScreenLayer
        readonly property alias overlays: overlaysLayer
        readonly property alias notifications: notificationsLayer
    }

    readonly property var panel: shellLoader.item ? shellLoader.item.panel : null

    id: root
    transform: Scale {
        id: screenScaler
        origin.x: zoomArea.x2
        origin.y: zoomArea.y2
        xScale: zoomArea.zoom2
        yScale: zoomArea.zoom2
    }

    /*
     * Output information panel
     */

    OutputInfo {
        id: outputInfo
        anchors {
            left: parent.left
            top: parent.top
        }
        number: root.number
        primary: root.primary
        visible: false
        z: 4000
    }

    /*
     * Screen zoom handler
     */

    ScreenZoom {
        id: zoomArea
        anchors.fill: parent
        scaler: screenScaler
        enabled: true
        z: 3000
    }

    /*
     * Hot corners
     */

    HotCorners {
        id: hotCorners
        anchors.fill: parent
        z: 2000
        onTopLeftTriggered: workspacesLayer.selectPrevious()
        onTopRightTriggered: workspacesLayer.selectNext()
        onBottomLeftTriggered: compositorRoot.toggleEffect("PresentWindowsGrid")
    }

    /*
     * Workspace
     */

    // Background is below everything
    Background {
        id: backgroundLayer
        anchors.fill: parent

        // Desktop is only above background
        Desktop {
            id: desktopLayer
            anchors.fill: parent
            z: 0
        }

        // Workspaces
        WorkspacesView {
            id: workspacesLayer
            anchors.fill: parent
            z: 1
        }

        // Panels
        Loader {
            id: shellLoader
            anchors.fill: parent
            active: primary
            sourceComponent: Shell {
                onPanelHeightChanged: setAvailableGeometry(panel.height)
            }
            z: 2
            visible: status == Loader.Ready
        }

        // Full screen windows can cover application windows and panels
        Rectangle {
            id: fullScreenLayer
            anchors.fill: parent
            color: "black"
            z: 10
            opacity: children.length > 0 ? 1.0 : 0.0

            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.InSine
                    duration: Themes.Units.mediumDuration
                }
            }
        }

        // Overlays are above the panel
        Overlay {
            id: overlaysLayer
            anchors.centerIn: parent
            z: 4
        }

        // Notifications are behind the panel
        Item {
            id: notificationsLayer
            anchors.fill: parent
            z: 4
        }
    }

    function setAvailableGeometry(h) {
        // Set available geometry so that windows are maximized properly
        var pt = _greenisland_output.mapToGlobal(0, 0);
        var g = Qt.rect(pt.x, pt.y, _greenisland_output.geometry.width, _greenisland_output.geometry.height - h);
        _greenisland_output.availableGeometry = g;
        console.debug("Available geometry for", name, "is:", _greenisland_output.availableGeometry);
    }
}
