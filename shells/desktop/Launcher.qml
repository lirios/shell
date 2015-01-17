/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0 as KQuickControls
import org.hawaii.launcher 0.1 as Launcher
import Hawaii.Components 1.0 as Components

Item {
    property real iconSize: units.iconSizes.large
    readonly property real itemSize: iconSize + (iconSize * 0.25)
    readonly property real itemPadding: units.smallSpacing * 2
    property alias orientation: listView.orientation

    id: launcher

    Component {
        id: iconDelegate

        Item {
            property int badgeCount: 1

            width: itemSize
            height: width

            Behavior on width {
                NumberAnimation {
                    easing.type: Easing.Linear
                    duration: units.shortDuration
                }
            }

            Behavior on height {
                NumberAnimation {
                    easing.type: Easing.Linear
                    duration: units.shortDuration
                }
            }

            Components.Icon {
                anchors.centerIn: parent
                iconName: model.iconName
                width: iconSize
                height: width
                onStatusChanged: {
                    // Fallback to a generic icon
                    if (status == Image.Error)
                        iconName = "binary";
                }
            }

            Rectangle {
                id: badge
                anchors {
                    top: parent.top
                    right: parent.right
                    topMargin: -(units.smallSpacing * 0.35)
                    rightMargin: -(units.largeSpacing * 0.35)
                }
                width: units.iconSizes.smallMedium
                height: width
                radius: width * 0.5
                color: "orangered"
                opacity: model.hasCount > 0 ? 1.0 : 0.0

                Behavior on opacity {
                    NumberAnimation {
                        easing.type: Easing.OutQuad
                        duration: units.shortDuration
                    }
                }

                Label {
                    anchors.centerIn: parent
                    font.pixelSize: parent.width - units.smallSpacing
                    color: "white"
                    text: model.count
                }
            }
        }
    }

    ListView {
        id: listView
        anchors.centerIn: parent
        orientation: ListView.Horizontal
        spacing: units.smallSpacing
        interactive: false
        add: Transition {
            NumberAnimation { properties: "scale"; from: 0.1; to: 1.0; duration: units.shortDuration }
        }
        populate: Transition {
            NumberAnimation { properties: "scale"; from: 0.1; to: 1.0; duration: units.longDuration }
        }
        model: Launcher.LauncherModel {
            id: launcherModel
        }
        delegate: iconDelegate
        width: calcWidth()
        height: calcHeight()

        function calcWidth() {
            if (orientation == ListView.Horizontal)
                return contentWidth;
            return itemSize + (itemPadding / 2);
        }

        function calcHeight() {
            if (orientation == ListView.Vertical)
                return contentHeight;
            return itemSize + (itemPadding / 2);
        }
    }
}
