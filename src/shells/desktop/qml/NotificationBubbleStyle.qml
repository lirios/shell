/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Shell.Styles 0.1
import FluidUi 0.2 as FluidUi
import FluidExtra 0.2 as FluidExtra

Style {
    property color panelColor: Qt.rgba(0, 0, 0, 0.7)

    padding {
        left: 2
        top: 2
    }

    property Component summary: Text {
        text: item.summary
        color: "white"
        style: Text.Raised
        styleColor: "#4000000"
        renderType: Text.NativeRendering
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        maximumLineCount: 2
        wrapMode: Text.Wrap
    }

    property Component body: Text {
        text: item.body
        color: "white"
        style: Text.Raised
        styleColor: "#4000000"
        renderType: Text.NativeRendering
        verticalAlignment: Text.AlignVCenter
        maximumLineCount: 20
        wrapMode: Text.Wrap
        textFormat: Text.StyledText
    }

    property Component image: Item {
        implicitWidth: 48
        implicitHeight: 48

        FluidUi.Icon {
            id: icon
            anchors.fill: parent
            iconName: item.iconName
            width: 48
            height: 48
            visible: iconName !== ""
        }

        FluidExtra.ImageItem {
            id: image
            anchors.fill: parent
            smooth: true
            image: item.picture
            visible: !icon.visible
        }
    }

    property Component panel: Item {
        implicitWidth: background.width + padding.left + padding.right
        implicitHeight: background.height + padding.top + padding.bottom

        Rectangle {
            id: background
            anchors {
                fill: parent
                leftMargin: padding.left
                topMargin: padding.top
                rightMargin: padding.right
                bottomMargin: padding.bottom
            }
            color: panelColor
            radius: 6

            RowLayout {
                anchors.fill: parent

                Loader {
                    id: imageLoader
                    sourceComponent: image
                }

                ColumnLayout {
                    Loader {
                        id: summaryLoader
                        sourceComponent: summary
                    }

                    Loader {
                        id: bodyLoader
                        sourceComponent: body
                    }
                }
            }
        }
    }
}
