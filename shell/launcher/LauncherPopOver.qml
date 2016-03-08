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

import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Controls 1.0 as Controls
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import Fluid.Ui 1.0 as FluidUi
import "../components" as CustomComponents

Components.Showable {
    property var origin
    property int location

    signal appLaunched()
    signal dismissed()

    id: root
    showAnimation: ParallelAnimation {
        XAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: FluidUi.Units.longDuration
            from: __priv.fromX
            to: __priv.toX
        }

        YAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: FluidUi.Units.longDuration
            from: __priv.fromY
            to: __priv.toY
        }

        OpacityAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: FluidUi.Units.longDuration
            from: 0.0
            to: 1.0
        }
    }
    hideAnimation: ParallelAnimation {
        XAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: FluidUi.Units.shortDuration
            from: __priv.toX
            to: __priv.fromX
        }

        YAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: FluidUi.Units.shortDuration
            from: __priv.toY
            to: __priv.fromY
        }

        OpacityAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: FluidUi.Units.longDuration
            from: 1.0
            to: 0.0
        }
    }

    QtObject {
        id: __priv

        property real fromX: origin.x + FluidUi.Units.dp(5) // 5 is an arbitrary margin
        property real fromY: origin.y + popover.height + FluidUi.Units.dp(5 + 6) // 6 is radius

        property real toX: fromX
        property real toY: origin.y - popover.height
    }

    CustomComponents.Popover {
        id: popover
        width: grid.width + (categories.visible ? categories.width : 0) + (FluidUi.Units.largeSpacing * 2) + (categories.visible ? FluidUi.Units.smallSpacing * 2 : 0)
        height: searchBox.height + grid.height + pageSelector.height +
                shutdownActions.height + (FluidUi.Units.largeSpacing * 4) +
                (categories.visible ? FluidUi.Units.smallSpacing : FluidUi.Units.largeSpacing) +
                FluidUi.Units.smallSpacing
        opacity: 0.0
        onOpacityChanged: if (opacity == 1.0) searchText.forceActiveFocus()

        Controls.PopupBehavior {
            anchors.fill: parent
            onDismissed: root.dismissed()
        }

        MouseArea {
            anchors.fill: parent
            onWheel: {
                if (wheel.angleDelta.x > 0 || wheel.angleDelta.y > 0) {
                    // Go to the next page
                    if (grid.currentPage < grid.numPages - 1) {
                        grid.currentPage++;
                        grid.currentIndex = grid.currentPage * grid.numItemsPerPage + 1;
                    }
                } else if (wheel.angleDelta.x < 0 || wheel.angleDelta.y < 0) {
                    // Go to the next page
                    if (grid.currentPage > 0) {
                        grid.currentPage--;
                        grid.currentIndex = grid.currentPage * grid.numItemsPerPage + 1;
                    }
                }

                wheel.accepted = true;
            }
        }

        RowLayout {
            id: searchBox
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                leftMargin: FluidUi.Units.largeSpacing
                topMargin: FluidUi.Units.largeSpacing
                rightMargin: FluidUi.Units.largeSpacing
                bottomMargin: FluidUi.Units.smallSpacing
            }
            height: Math.max(viewSelector.height, searchText.height)

            Row {
                id: viewSelector

                ExclusiveGroup {
                    id: viewGroup
                }

                ToolButton {
                    iconName: "view-more-symbolic"
                    checkable: true
                    checked: categories.visible
                    exclusiveGroup: viewGroup
                    onClicked: {
                        categories.currentIndex = 0;
                        categories.visible = true;
                    }
                }

                ToolButton {
                    iconName: "view-paged-symbolic"
                    checkable: true
                    checked: !categories.visible
                    exclusiveGroup: viewGroup
                    onClicked: {
                        categories.visible = false;
                        categories.currentIndex = 0;
                    }
                }
            }

            CustomComponents.TextField {
                id: searchText
                placeholderText: qsTr("Type an application name...")
                focus: true
                onTextChanged: grid.query = text

                Layout.fillWidth: true
            }
        }

        LauncherCategories {
            id: categories
            anchors {
                left: parent.left
                top: grid.top
                margins: FluidUi.Units.largeSpacing
            }
            width: FluidUi.Units.gu(8)
            height: grid.height
            visible: false
            onSelected: grid.filterByCategory(category)
        }

        LauncherGridView {
            id: grid
            anchors {
                left: categories.visible ? categories.right : parent.left
                top: searchBox.bottom
                leftMargin: categories.visible ? FluidUi.Units.smallSpacing : FluidUi.Units.largeSpacing
                topMargin: categories.visible ? FluidUi.Units.smallSpacing : FluidUi.Units.largeSpacing
                rightMargin: FluidUi.Units.largeSpacing
                bottomMargin: FluidUi.Units.largeSpacing
            }
            onAppLaunched: root.appLaunched()
        }

        Row {
            id: pageSelector
            anchors {
                top: grid.bottom
                horizontalCenter: parent.horizontalCenter
                leftMargin: FluidUi.Units.largeSpacing
                topMargin: FluidUi.Units.largeSpacing + FluidUi.Units.smallSpacing
                rightMargin: FluidUi.Units.largeSpacing
                bottomMargin: FluidUi.Units.largeSpacing
            }
            spacing: FluidUi.Units.largeSpacing
            height: FluidUi.Units.dp(20)

            Repeater {
                id: repeater
                model: grid.numPages
                delegate: Rectangle {
                    width: FluidUi.Units.dp(20)
                    height: width
                    radius: width
                    color: grid.currentPage === index
                           ? Themes.Theme.palette.panel.selectedBackgroundColor
                           : Themes.Theme.palette.rgba(Qt.lighter(Themes.Theme.palette.panel.backgroundColor, 1.5), 0.7)
                    antialiasing: true

                    Text {
                        anchors.centerIn: parent
                        text: index + 1
                        color: Themes.Theme.palette.panel.selectedTextColor
                        opacity: grid.currentPage === index ? 1.0 : 0.6
                        font.pixelSize: parent.width * 0.6
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            grid.currentIndex = index * grid.numItemsPerPage + 1
                            grid.currentPage = index;
                        }
                    }
                }
            }
        }

        LauncherShutdownActions {
            id: shutdownActions
            anchors {
                top: pageSelector.bottom
                horizontalCenter: parent.horizontalCenter
                leftMargin: FluidUi.Units.largeSpacing
                topMargin: FluidUi.Units.largeSpacing + FluidUi.Units.smallSpacing
                rightMargin: FluidUi.Units.largeSpacing
                bottomMargin: FluidUi.Units.largeSpacing
            }
        }
    }
}
