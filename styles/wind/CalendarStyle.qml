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
import QtQuick.Controls.Private 1.0 as QtControlsPrivate
import Hawaii.Themes 1.0 as Themes
import Hawaii.Controls 1.0 as Controls

QtControlsStyle.CalendarStyle {
    id: style
    gridColor: "transparent"
    gridVisible: false
    background: Item {
        implicitWidth: Themes.Units.gu(14)
        implicitHeight: Themes.Units.gu(20)
    }
    navigationBar: RowLayout {
        width: parent.width
        spacing: Themes.Units.smallSpacing

        ToolButton {
            //iconSize: Themes.Units.iconSizes.small
            iconName: "go-previous-symbolic"
            //flat: true
            onClicked: control.showPreviousMonth()

            Layout.alignment: Qt.AlignLeft
        }

        Controls.Heading {
            level: 3
            text: styleData.title
            font.capitalization: Font.Capitalize
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            color: Themes.Theme.palette.panel.textColor

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
        }

        ToolButton {
            //iconSize: Themes.Units.iconSizes.small
            iconName: "go-next-symbolic"
            //flat: true
            onClicked: control.showNextMonth()

            Layout.alignment: Qt.AlignRight
        }
    }
    dayDelegate: Item {
        Rectangle {
            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height)
            height: width
            radius: width / 2
            color: styleData.today ? Themes.Theme.palette.panel.primaryColor
                                   : styleData.selected ? Themes.Theme.palette.panel.selectedBackgroundColor : "transparent"
        }

        Text {
            id: label
            anchors.centerIn: parent
            color: {
                var theColor = Themes.Theme.palette.rgba(Themes.Theme.palette.panel.textColor, 0.5); //Qt.lighter(Themes.Theme.palette.panel.textColor, 1.4);
                if (styleData.valid) {
                    theColor = styleData.visibleMonth ? Qt.darker(Themes.Theme.palette.panel.textColor, 1.2) : Qt.lighter(Themes.Theme.palette.panel.textColor, 1.35);
                    if (styleData.selected)
                        theColor = Themes.Theme.palette.panel.selectedBackgroundColor;
                }
                return theColor;
            }
            text: styleData.date.getDate()
            renderType: QtControlsPrivate.Settings.isMobile ? Text.QtRendering : Text.NativeRendering
        }
    }
    dayOfWeekDelegate: Rectangle {
        color: style.gridVisible ? Themes.Theme.palette.rgba(Themes.Theme.palette.panel.backgroundColor, 0.5) : "transparent"
        implicitHeight: Themes.Theme.mSize(Themes.Theme.smallestFont).height

        Text {
            anchors.centerIn: parent
            text: control.__locale.dayName(styleData.dayOfWeek, control.dayOfWeekFormat)
            font: Themes.Theme.smallestFont
            color: Themes.Theme.palette.panel.textColor
            renderType: QtControlsPrivate.Settings.isMobile ? Text.QtRendering : Text.NativeRendering
        }
    }
    weekNumberDelegate: Item {
        implicitWidth: 2 * Themes.Theme.mSize(Themes.Theme.smallestFont).width + Themes.Units.smallSpacing

        Text {
            anchors.centerIn: parent
            text: styleData.weekNumber
            font: Themes.Theme.smallestFont
            color: Themes.Theme.palette.panel.textColor
            renderType: QtControlsPrivate.Settings.isMobile ? Text.QtRendering : Text.NativeRendering
        }
    }
}
