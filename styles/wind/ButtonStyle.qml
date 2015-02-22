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

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1 as QtControlsStyle
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0

QtControlsStyle.ButtonStyle {
    property int minimumWidth: Layout.minimumWidth + style.padding.left + style.padding.right
    property int minimumHeight: Layout.minimumHeight + style.padding.left + style.padding.right
    property size mSize: Theme.mSize(Theme.defaultFont)
    property bool pressed: control.pressed || control.checked

    padding {
        left: units.smallSpacing
        top: units.smallSpacing
        right: units.smallSpacing
        bottom: units.smallSpacing
    }

    id: style
    label: RowLayout {
        spacing: units.smallSpacing

        Components.Icon {
            readonly property bool valid: status === Image.Ready || status === Image.Loading

            id: icon
            anchors.verticalCenter: parent.verticalCenter
            iconName: control.__action.iconName ? control.__action.iconName : control.iconName
            iconSource: {
                if (control.__action && !control.__action.iconName)
                    return control.__action.iconSource;
                if (control.iconSource)
                    return control.iconSource;
                return "";
            }
            color: Theme.palette.panel.textColor
            width: control.iconSize > 0 ? control.iconSize : units.iconSizes.medium
            height: width
            visible: valid
        }

        Label {
            id: label
            text: control.text
            visible: control.text != ""
            height: parent.height
            color: Theme.palette.panel.textColor
            horizontalAlignment: icon.valid ? Text.AlignLeft : Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight

            Layout.fillWidth: true
            Layout.minimumWidth: implicitWidth
        }

        Loader {
            id: arrow
            active: control.menu !== null
            height: width
            sourceComponent: Image {
                anchors.fill: parent
                source: "images/arrow-down.png"
            }
            opacity: control.enabled ? 0.6 : 0.5
            visible: active

            Layout.minimumWidth: units.iconSizes.small
            Layout.maximumWidth: Layout.minimumWidth
            Layout.alignment: Qt.AlignVCenter
        }

        Layout.preferredHeight: Math.max(units.iconSizes.small, label.implicitHeight)
    }
    background: Item {
        implicitWidth: control.text.length == 0 ? height : Math.max(style.mSize.width * 12, style.minimumWidth)
        implicitHeight: Math.floor(Math.max(style.mSize.height * 1.6, style.minimumHeight))

        Loader {
            anchors.fill: parent
            active: !control.flat
            sourceComponent: normalComponent
            visible: active
        }
    }

    Component {
        id: normalComponent

        Rectangle {
            property color c: Theme.palette.panel.backgroundColor

            id: baserect
            anchors.fill: parent
            border.color: Qt.darker(baserect.c, 1.2)
            radius: 4
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: style.pressed ? Qt.darker(baserect.c, 1.4) : Qt.lighter(baserect.c, 1.8)
                }
                GradientStop {
                    position: 0.1
                    color: style.pressed ? Qt.darker(baserect.c, 1.15) : Qt.lighter(baserect.c, 1.4)
                }
                GradientStop {
                    position: 0.8
                    color: style.pressed ? Qt.darker(baserect.c, 0.76) : baserect.c
                }
                GradientStop {
                    position: 1.0
                    color: pressed ? Qt.darker(baserect.c, 0.65) : Qt.darker(baserect.c, 1.3)
                }
            }
            antialiasing: true
        }
    }
}
