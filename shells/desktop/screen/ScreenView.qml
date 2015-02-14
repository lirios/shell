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
import QtQuick.Window 2.0
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0
import GreenIsland 1.0
import Hawaii.Themes 1.0 as Themes
import org.hawaii.misc 0.1 as Misc
import ".."
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
        readonly property alias notifications: notificationsLayer
    }

    readonly property alias panel: panel

    id: root
    transform: Scale {
        id: screenScaler
        origin.x: zoomArea.x2
        origin.y: zoomArea.y2
        xScale: zoomArea.zoom2
        yScale: zoomArea.zoom2
    }

    QtObject {
        id: __priv

        readonly property int lowIndex: 0
        readonly property int highIndex: 2000
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
     * Workspace
     */

    // Background is below everything
    Image {
        id: backgroundLayer
        anchors.fill: parent
        source: Misc.StandardPaths.locateFile(Misc.StandardPaths.GenericDataLocation, "backgrounds/hawaii/Also_Calm.png")
        sourceSize.width: width
        sourceSize.height: height
        cache: false

        // Desktop is only above background
        Desktop {
            id: desktopLayer
            anchors.fill: parent
            z: 0
            onClockClicked: topDrawer.toggle()
        }

        // Workspaces
        WorkspacesLinearView {
            id: workspacesLayer
            anchors.fill: parent
            z: 1
        }

        // Full screen windows can cover application windows and panels
        Item {
            id: fullScreenLayer
            anchors.fill: parent
            visible: false
            z: 10
        }

        // Notifications are behind the panel
        Item {
            id: notificationsLayer
            anchors.fill: parent
            z: 4
        }

        Panel {
            id: panel
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            z: 3
            onHeightChanged: setAvailableGeometry()
            onIndicatorTriggered: {
                // AppChooser special case
                if (indicator.name === "appchooser") {
                    // Load AppChooser component
                    if (leftDrawer.component == undefined)
                        leftDrawer.component = indicator.component;
                    leftDrawer.toggle();
                    return;
                }

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
            z: 2

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
            z: 2

            StackView {
                id: stackView
                anchors.fill: parent
                anchors.margins: Themes.Units.largeSpacing
            }
        }

        SlidingPanel {
            id: topDrawer
            edge: Qt.TopEdge
            height: Themes.Units.gu(15)
            z: 2

            Loader {
                anchors.fill: parent
                anchors.margins: Themes.Units.largeSpacing
                active: topDrawer.expanded
                source: "../controlcenter/ControlCenter.qml"

                Connections {
                    target: loader.item
                    onClosed: topDrawer.close()
                }
            }
        }
    }

    // Hot corners
    HotCorners {
        id: hotCorners
        anchors.fill: parent
        rotation: 0
        onTopLeftTriggered: workspacesLayer.selectPrevious()
        onTopRightTriggered: workspacesLayer.selectNext()
        onBottomLeftTriggered: compositorRoot.toggleEffect("PresentWindowsGrid")
    }

    function setAvailableGeometry() {
        // Set available geometry so that windows are maximized properly
        var pt = _greenisland_output.mapToGlobal(0, 0);
        var g = Qt.rect(pt.x, pt.y, _greenisland_output.geometry.width, _greenisland_output.geometry.height - panel.height);
        _greenisland_output.availableGeometry = g;
        console.debug("Available geometry for", name, "is:", _greenisland_output.availableGeometry);
    }

    Component.onCompleted: setAvailableGeometry()
}
