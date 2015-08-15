/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Controls 1.0 as Controls
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
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
            duration: Themes.Units.longDuration
            from: __priv.fromX
            to: __priv.toX
        }

        YAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: Themes.Units.longDuration
            from: __priv.fromY
            to: __priv.toY
        }

        OpacityAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: Themes.Units.longDuration
            from: 0.0
            to: 1.0
        }
    }
    hideAnimation: ParallelAnimation {
        XAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: Themes.Units.shortDuration
            from: __priv.toX
            to: __priv.fromX
        }

        YAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: Themes.Units.shortDuration
            from: __priv.toY
            to: __priv.fromY
        }

        OpacityAnimator {
            target: popover
            easing.type: Easing.InQuad
            duration: Themes.Units.longDuration
            from: 1.0
            to: 0.0
        }
    }

    QtObject {
        id: __priv

        property real fromX: origin.x + Themes.Units.dp(5) // 5 is an arbitrary margin
        property real fromY: origin.y + popover.height + Themes.Units.dp(5 + 6) // 6 is radius

        property real toX: fromX
        property real toY: origin.y - popover.height
    }

    CustomComponents.Popover {
        id: popover
        width: grid.width + (categories.visible ? categories.width : 0) + (Themes.Units.largeSpacing * 2) + (categories.visible ? Themes.Units.smallSpacing * 2 : 0)
        height: searchBox.height + grid.height + pageSelector.height +
                shutdownActions.height + (Themes.Units.largeSpacing * 4) +
                (categories.visible ? Themes.Units.smallSpacing : Themes.Units.largeSpacing) +
                Themes.Units.smallSpacing
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
                leftMargin: Themes.Units.largeSpacing
                topMargin: Themes.Units.largeSpacing
                rightMargin: Themes.Units.largeSpacing
                bottomMargin: Themes.Units.smallSpacing
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
                margins: Themes.Units.largeSpacing
            }
            width: Themes.Units.gu(8)
            height: grid.height
            visible: false
            onSelected: grid.filterByCategory(category)
        }

        LauncherGridView {
            id: grid
            anchors {
                left: categories.visible ? categories.right : parent.left
                top: searchBox.bottom
                leftMargin: categories.visible ? Themes.Units.smallSpacing : Themes.Units.largeSpacing
                topMargin: categories.visible ? Themes.Units.smallSpacing : Themes.Units.largeSpacing
                rightMargin: Themes.Units.largeSpacing
                bottomMargin: Themes.Units.largeSpacing
            }
            onAppLaunched: root.appLaunched()
        }

        Row {
            id: pageSelector
            anchors {
                top: grid.bottom
                horizontalCenter: parent.horizontalCenter
                leftMargin: Themes.Units.largeSpacing
                topMargin: Themes.Units.largeSpacing + Themes.Units.smallSpacing
                rightMargin: Themes.Units.largeSpacing
                bottomMargin: Themes.Units.largeSpacing
            }
            spacing: Themes.Units.largeSpacing
            height: Themes.Units.dp(20)

            Repeater {
                id: repeater
                model: grid.numPages
                delegate: Rectangle {
                    width: Themes.Units.dp(20)
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
                leftMargin: Themes.Units.largeSpacing
                topMargin: Themes.Units.largeSpacing + Themes.Units.smallSpacing
                rightMargin: Themes.Units.largeSpacing
                bottomMargin: Themes.Units.largeSpacing
            }
        }
    }
}
