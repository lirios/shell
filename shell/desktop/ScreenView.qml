/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
        readonly property alias panels: shellLoader.item
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
        z: 0
    }

    // Desktop is only above background
    Desktop {
        id: desktopLayer
        anchors.fill: parent
        z: 1
    }

    // Workspaces
    WorkspacesView {
        id: workspacesLayer
        anchors.fill: parent
        z: 2
    }

    // Panels
    Loader {
        id: shellLoader
        anchors.fill: parent
        active: primary
        sourceComponent: Shell {
            onPanelHeightChanged: setAvailableGeometry(panel.height)
        }
        z: 3
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
        z: 5
    }

    // Notifications are behind the panel
    Item {
        id: notificationsLayer
        anchors.fill: parent
        z: 5
    }

    function setAvailableGeometry(h) {
        // Set available geometry so that windows are maximized properly
        var pt = _greenisland_output.mapToGlobal(0, 0);
        var g = Qt.rect(pt.x, pt.y, _greenisland_output.geometry.width, _greenisland_output.geometry.height - h);
        _greenisland_output.availableGeometry = g;
        console.debug("Available geometry for", name, "is:", _greenisland_output.availableGeometry);
    }
}
