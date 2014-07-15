/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Shell.Styles.Base 1.0

NotificationBubbleStyle {
    spacing: 8
    panelColor1: Qt.rgba(0.13, 0.13, 0.13, 1)
    panelColor2: Qt.rgba(0, 0, 0, 1)
    textColor: "white"
    textShadowColor: Qt.rgba(0, 0, 0, 0.7)

    summary: Text {
        text: __item.summary
        color: textColor
        style: Text.Raised
        styleColor: textShadowColor
        renderType: Text.NativeRendering
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        maximumLineCount: 2
        wrapMode: Text.Wrap
    }

    body: Text {
        text: __item.body
        color: textColor
        style: Text.Raised
        styleColor: textShadowColor
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        maximumLineCount: 20
        wrapMode: Text.Wrap
        textFormat: Text.StyledText
    }

    image: Item {
        width: visible ? 48 : 0
        height: visible ? 48 : 0
        visible: icon.visible

        Image {
            id: icon
            source: __item.iconName
            sourceSize.width: icon.width
            sourceSize.height: icon.height
            width: 48
            height: 48
            smooth: true
            visible: source !== ""
        }
    }

    panel: Item {
        implicitWidth: 240 + padding.left + padding.right + (spacing * 3)
        implicitHeight: summaryLoader.height + bodyLoader.height + padding.top + padding.bottom + (spacing * 3)

        Rectangle {
            id: background
            x: padding.left
            y: padding.top
            width: parent.width - padding.left - padding.right
            height: parent.height - padding.top - padding.bottom
            border.color: Qt.rgba(0, 0, 0, 0.5)
            gradient: Gradient {
                GradientStop { position: 0.0; color: panelColor1 }
                GradientStop { position: 0.5; color: panelColor2 }
                GradientStop { position: 1.0; color: panelColor2 }
            }
            radius: 6
            opacity: 0.7
            antialiasing: true
        }

        Item {
            anchors.fill: background
            Loader {
                id: imageLoader
                anchors {
                    left: parent.left
                    top: parent.top
                    leftMargin: spacing
                    topMargin: spacing
                }
                sourceComponent: image
            }

            Loader {
                id: summaryLoader
                anchors {
                    left: imageLoader.right
                    top: imageLoader.top
                    right: parent.right
                    leftMargin: spacing
                    rightMargin: spacing
                }
                sourceComponent: summary
            }

            Loader {
                id: bodyLoader
                anchors {
                    left: imageLoader.right
                    top: summaryLoader.bottom
                    right: parent.right
                    leftMargin: spacing
                    rightMargin: spacing
                }
                sourceComponent: body
            }
        }
    }
}
