/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2 as QtControlsStyle
import Hawaii.Themes 1.0

QtControlsStyle.CalendarStyle {
    id: style
    background: Item {}
    gridColor: Theme.palette.rgba(Theme.palette.panel.textColor, 0.3)
    gridVisible: true
    navigationBar: RowLayout {
        width: parent.width
        spacing: units.smallSpacing

        ToolButton {
            iconSize: units.iconSizes.small
            iconName: "go-previous-symbolic"
            flat: true
            onClicked: control.showPreviousMonth()

            Layout.alignment: Qt.AlignLeft
        }

        Components.Heading {
            level: 3
            text: styleData.title
            font.capitalization: Font.Capitalize
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
        }

        ToolButton {
            iconSize: units.iconSizes.small
            iconName: "go-next-symbolic"
            flat: true
            onClicked: control.showNextMonth()

            Layout.alignment: Qt.AlignRight
        }
    }
    dayDelegate: Item {
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            anchors.margins: styleData.selected ? -1 : 0
            border.color: "transparent"
            color: styleData.date !== undefined && styleData.selected ? Theme.palette.panel.selectedBackgroundColor : "transparent"
        }

        Label {
            id: label
            anchors.centerIn: parent
            color: {
                var theColor = Theme.palette.rgba(Theme.palette.panel.textColor, 0.5); //Qt.lighter(Theme.palette.panel.textColor, 1.4);
                if (styleData.valid) {
                    theColor = styleData.visibleMonth ? Qt.darker(Theme.palette.panel.textColor, 1.2) : Qt.ligther(Theme.palette.panel.textColor, 1.35);
                    if (styleData.selected)
                        theColor = Theme.palette.panel.selectedBackgroundColor;
                }
                return theColor;
            }
            text: styleData.date.getDate()
        }
    }
    dayOfWeekDelegate: Rectangle {
        color: style.gridVisible ? Theme.palette.rgba(Theme.palette.panel.backgroundColor, 0.5) : "transparent"
        implicitHeight: Theme.mSize(Theme.smallestFont).height

        Label {
            anchors.centerIn: parent
            text: control.__locale.dayName(styleData.dayOfWeek, control.dayOfWeekFormat)
            font: Theme.smallestFont
            color: Theme.palette.panel.textColor
        }
    }
    weekNumberDelegate: Item {
        implicitWidth: 2 * Theme.mSize(Theme.smallestFont).width + units.smallSpacing

        Label {
            anchors.centerIn: parent
            text: styleData.weekNumber
            font: Theme.smallestFont
            color: Theme.palette.panel.textColor
        }
    }
}
