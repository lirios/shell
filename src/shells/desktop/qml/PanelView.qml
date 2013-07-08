/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.1
import QtQuick.Controls 1.0
import GreenIsland 1.0

Item {
    id: panelView

    // Spacing between indicators
    property real spacing: 8

    Item {
        id: leftBox
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: Math.min(childrenRect.width, parent.width / 3)

        WorkspaceIndicator {
            id: workspaceIndicator
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                rightMargin: spacing
            }
        }
    }

    Item {
        id: centerBox
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        width: parent.width / 3

        DateIndicator {
            id: dateIndicator
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                rightMargin: spacing
            }
        }
    }

    Item {
        id: rightBox
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
        width: Math.min(childrenRect.width, parent.width / 3)

        /*
        Row {
            id: trayArea
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            Repeater {
                model: IndicatorsModel {}

                PanelIndicator {
                    label: indicator.label
                    iconName: indicator.iconName
                    panelView: panelView
                }
            }
        }
        */

        NetworkIndicator {
            id: networkIndicator
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                rightMargin: spacing
            }
        }
        UserIndicator {
            id: userIndicator
            anchors {
                left: networkIndicator.right
                top: parent.top
                bottom: parent.bottom
                rightMargin: spacing
            }
        }
        NotificationsIndicator {
            id: notificationsIndicator
            anchors {
                left: userIndicator.right
                top: parent.top
                bottom: parent.bottom
                rightMargin: spacing
            }
        }
    }
}
