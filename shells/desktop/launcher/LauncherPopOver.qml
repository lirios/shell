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

import QtQuick 2.2
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
        width: grid.width + categories.width + (Themes.Units.largeSpacing * 2) + (categories.visible ? Themes.Units.smallSpacing * 2 : 0)
        height: grid.height + pageSelector.height + shutdownActions.height + (Themes.Units.largeSpacing * 4) + Themes.Units.smallSpacing
        opacity: 0.0

        Controls.PopupBehavior {
            anchors.fill: parent
            onDismissed: root.dismissed()
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
        }

        LauncherGridView {
            id: grid
            anchors {
                left: categories.right
                top: parent.top
                leftMargin: categories.visible ? Themes.Units.smallSpacing : Themes.Units.largeSpacing
                topMargin: Themes.Units.largeSpacing
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
