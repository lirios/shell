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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import "plasmapackage:/"
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

    RowLayout {
        spacing: units.largeSpacing

        Icon {
            id: icon
            width: units.iconSizes.medium
            height: width
            iconName: appIcon && appIcon.length > 0 ? appIcon : "dialog-information-symbolic"
            color: Theme.panel.textColor

            Layout.alignment: Qt.AlignTop
        }

        ColumnLayout {
            spacing: units.largeSpacing

            Label {
                clip: true
                text: summary + (root.expanded ? (body ? "\n" + body : "") :
                                                 (body ? "..." : ""))
                color: Theme.panel.textColor
                font.pointSize: 14
            }

            Item {
                id: extraArea
                width: root.parent.width
                height: root.parent.height
                visible: root.expanded
            }
        }
    }
}
