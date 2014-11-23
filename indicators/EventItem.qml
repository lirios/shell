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
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import ".."
import "../components"

MouseArea {
    property bool expanded: false

    id: root
    width: parent.width
    height: units.gridUnit * 2
    drag.axis: Drag.XAxis
    drag.target: root
    onExpandedChanged: {
        if (expanded && body)
            height = units.gridUnit * 4;
        else
            height = units.gridUnit * 2;
    }
    onReleased: {
        if (drag.active) {
            if (x > width / 4 || x < width / -4)
                notificationsModel.remove(index);
            else
                x = 0;
        } else if (body) {
            expanded = !expanded;
        }
    }

    Behavior on x {
        SpringAnimation {
            spring: 2
            damping: 0.2
        }
    }

    Behavior on height {
        SpringAnimation {
            spring: 5
            damping: 0.3
        }
    }

    Icon {
        id: icon
        anchors.verticalCenter: parent.verticalCenter
        x: units.largeSpacing
        y: 0
        width: units.iconSizes.medium
        height: width
        iconName: appIcon && appIcon.length > 0 ? appIcon : "dialog-information-symbolic"
        color: Theme.panel.textColor
        visible: !root.expanded
    }

    Item {
        id: summaryArea
        anchors {
            left: root.expanded ? root.left : root.right
            top: root.top
            right: root.right
        }
        width: parent.width - icon.width - (units.largeSpacing * 2)
        height: parent.height

        PlasmaExtras.Title {
            anchors.fill: parent
            clip: true
            horizontalAlignment: root.expanded ? Qt.AlignHCenter : Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            text: summary + (root.expanded ? (body ? "\n" + body : "") :
                                             (body ? "..." : ""))
            color: Theme.panel.textColor
        }
    }

    Item {
        id: extraArea
        anchors {
            left: summaryArea.right
            top: parent.top
            bottom: parent.bottom
        }
        width: parent.width
        height: parent.width
    }
}
