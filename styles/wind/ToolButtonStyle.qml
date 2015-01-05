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

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1 as QtControlsStyle
import Hawaii.Components 1.0 as Components
import QtQuick.Controls.Styles.Wind 1.0 as Wind
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import "private/Utils.js" as Utils

QtControlsStyle.ButtonStyle {
    property int minimumWidth: Layout.minimumWidth + style.padding.left + style.padding.right
    property int minimumHeight: Layout.minimumHeight + style.padding.left + style.padding.right
    property size mSize: theme.mSize(theme.defaultFont)
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
            //anchors.verticalCenter: parent.verticalCenter
            //anchors.horizontalCenter: label.text.length > 0 ? undefined : parent.horizontalCenter
            iconName: control.__action.iconName ? control.__action.iconName : control.iconName
            iconSource: {
                if (control.__action && !control.__action.iconName)
                    return control.__action.iconSource;
                if (control.iconSource)
                    return control.iconSource;
                return "";
            }
            color: Wind.Palette.panel.textColor
            width: control.iconSize > 0 ? control.iconSize : units.iconSizes.medium
            height: width
            visible: valid

            Layout.alignment: Qt.AlignCenter
        }

        PlasmaComponents.Label {
            id: label
            text: control.text
            visible: control.text != "" && !icon.visible
            height: parent.height
            color: Wind.Palette.panel.textColor
            //horizontalAlignment: icon.valid ? Text.AlignLeft : Text.AlignHCenter
            //horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight

            //Layout.fillWidth: true
            Layout.minimumWidth: implicitWidth
            Layout.alignment: Qt.AlignCenter
        }

        PlasmaExtras.ConditionalLoader {
            id: arrow
            when: control.menu !== null
            height: width
            source: Component {
                PlasmaCore.SvgItem {
                    anchors.fill: parent
                    svg: PlasmaCore.Svg {
                        imagePath: "widgets/arrows"
                        colorGroup: control.hovered || !control.flat ? PlasmaCore.Theme.ButtonColorGroup : PlasmaCore.Theme.NormalColorGroup
                    }
                    elementId: "down-arrow"
                    visible: control.menu !== null
                }
            }
            visible: when

            Layout.minimumWidth: units.iconSizes.small
            Layout.maximumWidth: Layout.minimumWidth
            Layout.alignment: Qt.AlignVCenter
        }

        Layout.preferredHeight: Math.max(units.gridUnit, Math.max(icon.visible ? icon.height : 0, label.visible ? label.implicitHeight : 0))
    }
    background: Item {
        implicitWidth: (!label.visible || control.text.length == 0) ? height : Math.max(style.mSize.width * 12, style.minimumWidth)
        implicitHeight: Math.floor(Math.max(style.mSize.height * 1.6, style.minimumHeight))

        PlasmaExtras.ConditionalLoader {
            anchors.fill: parent
            when: !control.flat
            source: normalComponent
            visible: when
        }
    }

    Component {
        id: normalComponent

        Rectangle {
            property color c: Wind.Palette.panel.backgroundColor

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
