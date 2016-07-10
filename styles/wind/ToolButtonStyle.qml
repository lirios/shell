/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls.Private 1.0 as QtControlsPrivate
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes

QtControlsPrivate.ToolButtonStyle {
    id: style
    panel: Item {
        id: styleitem
        implicitWidth: (hasIcon ? icon.width + Themes.Units.smallSpacing : 0) +
                       label.paintedWidth +
                       (arrow.visible ? arrow.width + Themes.Units.smallSpacing : 0) +
                       frame.border.left + frame.border.right +
                       (2 * Themes.Units.smallSpacing)
        implicitHeight: Math.max(hasIcon ? icon.height : 0, label.paintedHeight) +
                        (3 * Themes.Units.smallSpacing)

        readonly property bool hasIcon: icon.status === Image.Ready || icon.status === Image.Loading
        readonly property real buttonRadius: Themes.Units.dp(4)

        Component {
            id: normalComponent

            Rectangle {
                property color c: Themes.Theme.palette.panel.backgroundColor

                id: baserect
                anchors.fill: parent
                border.color: Qt.darker(baserect.c, 1.2)
                radius: buttonRadius
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

        Component {
            id: pressedComponent

            Rectangle {
                property color c: Themes.Theme.palette.panel.backgroundColor

                id: baserect
                anchors.fill: parent
                border.color: Qt.darker(baserect.c, 1.2)
                radius: buttonRadius
                gradient: Gradient {
                    GradientStop {
                        position: 1.0
                        color: style.pressed ? Qt.darker(baserect.c, 1.4) : Qt.lighter(baserect.c, 1.8)
                    }
                    GradientStop {
                        position: 0.8
                        color: style.pressed ? Qt.darker(baserect.c, 1.15) : Qt.lighter(baserect.c, 1.4)
                    }
                    GradientStop {
                        position: 0.1
                        color: style.pressed ? Qt.darker(baserect.c, 0.76) : baserect.c
                    }
                    GradientStop {
                        position: 0.0
                        color: pressed ? Qt.darker(baserect.c, 0.65) : Qt.darker(baserect.c, 1.3)
                    }
                }
                antialiasing: true
            }
        }

        Component {
            id: hoverComponent

            Rectangle {
                property color c: Themes.Theme.palette.panel.backgroundColor

                id: baserect
                anchors.fill: parent
                border.color: Qt.darker(baserect.c, 1.2)
                radius: buttonRadius
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

        Loader {
            anchors.fill: parent
            sourceComponent: {
                if (control.pressed || (control.checkable && control.checked))
                    return pressedComponent;
                else if (control.hovered)
                    return hoverComponent;
                return normalComponent;
            }
        }

        Item {
            anchors.left: parent.left
            anchors.right: arrow.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            clip: true

            Text {
                id: label
                visible: !hasIcon
                anchors.centerIn: parent
                text: control.text
                renderType: QtControlsPrivate.Settings.isMobile ? Text.QtRendering : Text.NativeRendering
                color: Themes.Theme.palette.panel.textColor
            }

            Components.Icon {
                id: icon
                anchors.centerIn: parent
                name: control.__action.iconName ? control.__action.iconName : control.iconName ? control.iconName : ""
                source: {
                    if (control.__action && !control.__action.iconName)
                        return control.__action.iconSource;
                    if (control.iconSource)
                        return control.iconSource;
                    return "";
                }
                color: Themes.Theme.palette.panel.textColor
                width: Themes.Units.iconSizes.smallMedium
                height: width
            }
        }

        BorderImage {
            id: frame
            anchors.fill: parent
            anchors.margins: -1
            anchors.topMargin: -2
            anchors.rightMargin: 0
            border.left: 4
            border.right: 4
            border.top: 4
            border.bottom: 4
            source: "images/focusframe.png"
            visible: control.activeFocus
        }

        Image {
            id: arrow
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: visible ? 3 : 0
            source: visible ? "images/arrow-down.png" : ""
            opacity: control.enabled ? 0.7 : 0.5
            visible: control.menu !== null
        }
    }
}
