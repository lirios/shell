/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.2
import QtQuick.Controls.Styles 1.1 as QtControlsStyle
import QtQuick.Controls.Private 1.0 as QtControlsPrivate
import Hawaii.Components 1.0 as Components
import QtQuick.Controls.Styles.Wind 1.0 as Wind
import org.kde.plasma.core 2.0 as PlasmaCore
import "private/Utils.js" as Utils

QtControlsStyle.ScrollViewStyle {
    id: style

    PlasmaCore.Svg {
        id: arrowsSvg
        imagePath: "widgets/arrows"
    }

    scrollBarBackground: Item {
        property bool sticky: false
        property bool hovered: styleData.hovered

        implicitWidth: units.gridUnit
        implicitHeight: units.gridUnit * 12
        clip: true
        opacity: transientScrollBars ? 0.5 : 1.0
        visible: !QtControlsPrivate.Settings.hasTouchScreen && (!transientScrollBars || sticky)
        onHoveredChanged: if (hovered) sticky = true
        onVisibleChanged: if (!visible) sticky = false

        Rectangle {
            anchors.fill: parent
            anchors.rightMargin: styleData.horizontal ? -2 : -1
            anchors.leftMargin: styleData.horizontal ? -2 : 0
            anchors.topMargin: styleData.horizontal ? 0 : -2
            anchors.bottomMargin: styleData.horizontal ? -1 : -2
            border.color: Qt.darker(Wind.Palette.panel.backgroundColor, 1.8)
            color: Qt.lighter(Wind.Palette.panel.backgroundColor, 1.5)
            radius: units.gridUnit * 0.6
        }
    }
    handle: Item {
        property bool sticky: false
        property bool hovered: __activeControl !== "none"

        implicitWidth: units.gridUnit + 1
        implicitHeight: units.gridUnit * 12 + 1
        onHoveredChanged: if (hovered) sticky = true
        onVisibleChanged: if (!visible) sticky = false

        Rectangle {
            anchors.top: !styleData.horizontal ? parent.top : undefined
            anchors.margins: transientScrollBars ? 2 : 0
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: styleData.horizontal ? parent.left : undefined
            border.width: transientScrollBars ? units.gridUnit * 0.15 : units.gridUnit * 0.05
            border.color: Qt.darker(Wind.Palette.panel.backgroundColor, 1.8)
            color: Qt.darker(Wind.Palette.panel.backgroundColor, 1.4)
            radius: units.gridUnit * 0.6
            width: !styleData.horizontal && transientScrollBars ? sticky ? units.gridUnit * 1.3 : units.gridUnit : parent.width
            height: styleData.horizontal && transientScrollBars ? sticky ? units.gridUnit * 1.3 : units.gridUnit : parent.height

            Behavior on width {
                enabled: !styleData.horizontal && transientScrollBars
                NumberAnimation { duration: units.longDuration }
            }

            Behavior on height {
                enabled: !styleData.horizontal && transientScrollBars
                NumberAnimation { duration: units.longDuration }
            }
        }
    }
    incrementControl: Item {
        implicitWidth: transientScrollBars ? 0 : units.gridUnit
        implicitHeight: transientScrollBars ? 0 : units.gridUnit
        visible: !transientScrollBars

        Item {
            anchors {
                fill: parent
                rightMargin: -1
                bottomMargin: -1
            }

            PlasmaCore.SvgItem {
                anchors.centerIn: parent
                anchors.margins: 1
                svg: arrowsSvg
                elementId: styleData.horizontal ? "right-arrow" : "down-arrow"
                width: units.gridUnit * 0.7
                height: width
            }
        }
    }
    decrementControl: Item {
        implicitWidth: transientScrollBars ? 0 : units.gridUnit
        implicitHeight: transientScrollBars ? 0 : units.gridUnit
        visible: !transientScrollBars

        Item {
            anchors {
                fill: parent
                leftMargin: styleData.horizontal ? -1 : 0
                topMargin: styleData.horizontal ? 0 : -1
                rightMargin: styleData.horizontal ? 0 : -1
                bottomMargin: styleData.horizontal ? -1 : 0
            }

            PlasmaCore.SvgItem {
                anchors.centerIn: parent
                anchors.margins: 1
                anchors.verticalCenterOffset: styleData.horizontal ? 0 : -1
                anchors.horizontalCenterOffset: styleData.horizontal ? -1 : 0
                svg: arrowsSvg
                elementId: styleData.horizontal ? "left-arrow" : "up-arrow"
                width: units.gridUnit * 0.7
                height: width
                opacity: control.enabled ? 0.6 : 0.5
            }
        }
    }
}
