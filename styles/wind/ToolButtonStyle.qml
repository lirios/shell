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
                iconName: control.__action.iconName ? control.__action.iconName : control.iconName ? control.iconName : ""
                iconSource: {
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
