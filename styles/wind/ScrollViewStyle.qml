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

import QtQuick 2.2
import QtQuick.Controls.Styles 1.1 as QtControlsStyle
import QtQuick.Controls.Private 1.0 as QtControlsPrivate
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

QtControlsStyle.ScrollViewStyle {
    id: style

    scrollBarBackground: Item {
        property bool sticky: false
        property bool hovered: styleData.hovered

        implicitWidth: Themes.Units.gu(1)
        implicitHeight: Themes.Units.gu(12)
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
            border.color: Qt.darker(Themes.Theme.palette.panel.backgroundColor, 1.8)
            color: Qt.lighter(Themes.Theme.palette.panel.backgroundColor, 1.5)
            radius: Themes.Units.gu(0.6)
        }
    }
    handle: Item {
        property bool sticky: false
        property bool hovered: __activeControl !== "none"

        implicitWidth: Themes.Units.gu(1) + 1
        implicitHeight: Themes.Units.gu(12) + 1
        onHoveredChanged: if (hovered) sticky = true
        onVisibleChanged: if (!visible) sticky = false

        Rectangle {
            anchors.top: !styleData.horizontal ? parent.top : undefined
            anchors.margins: transientScrollBars ? 2 : 0
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: styleData.horizontal ? parent.left : undefined
            border.width: transientScrollBars ? Themes.Units.gu(0.15) : Themes.Units.gu(0.05)
            border.color: Qt.darker(Themes.Theme.palette.panel.backgroundColor, 1.8)
            color: Qt.darker(Themes.Theme.palette.panel.backgroundColor, 1.4)
            radius: Themes.Units.gu(0.6)
            width: !styleData.horizontal && transientScrollBars ? sticky ? Themes.Units.gu(1.3) : Themes.Units.gu(1) : parent.width
            height: styleData.horizontal && transientScrollBars ? sticky ? Themes.Units.gu(1.3) : Themes.Units.gu(1) : parent.height

            Behavior on width {
                enabled: !styleData.horizontal && transientScrollBars
                NumberAnimation { duration: Themes.Units.longDuration }
            }

            Behavior on height {
                enabled: !styleData.horizontal && transientScrollBars
                NumberAnimation { duration: Themes.Units.longDuration }
            }
        }
    }
    incrementControl: Item {
        implicitWidth: transientScrollBars ? 0 : Themes.Units.gu(1)
        implicitHeight: transientScrollBars ? 0 : Themes.Units.gu(1)
        visible: !transientScrollBars

        Item {
            anchors {
                fill: parent
                rightMargin: -1
                bottomMargin: -1
            }

            Image {
                anchors.centerIn: parent
                source: styleData.horizontal ? "images/arrow-right.png" : "images/arrow-down.png"
            }
        }
    }
    decrementControl: Item {
        implicitWidth: transientScrollBars ? 0 : Themes.Units.gu(1)
        implicitHeight: transientScrollBars ? 0 : Themes.Units.gu(1)
        visible: !transientScrollBars

        Item {
            anchors {
                fill: parent
                leftMargin: styleData.horizontal ? -1 : 0
                topMargin: styleData.horizontal ? 0 : -1
                rightMargin: styleData.horizontal ? 0 : -1
                bottomMargin: styleData.horizontal ? -1 : 0
            }

            Image {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: styleData.horizontal ? 0 : -1
                anchors.horizontalCenterOffset: styleData.horizontal ? -1 : 0
                source: styleData.horizontal ? "images/arrow-left.png" : "images/arrow-up.png"
                opacity: control.enabled ? 0.6 : 0.5
            }
        }
    }
}
