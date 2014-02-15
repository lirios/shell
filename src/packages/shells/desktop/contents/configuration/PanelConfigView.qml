/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Hawaii.Shell 1.0

BaseConfigView {
    id: root

    ExclusiveGroup {
        id: positionGroup
    }

    ExclusiveGroup {
        id: sizeGroup
    }

    ExclusiveGroup {
        id: alignmentGroup
    }

    RowLayout {
        GroupBox {
            title: qsTr("Position")

            RowLayout {
                anchors.fill: parent

                ToolButton {
                    text: qsTr("Left")
                    iconName: "align-horizontal-left"
                    exclusiveGroup: positionGroup
                    checkable: true
                    checked: hawaiiShellQuickView.location === Types.LeftEdge
                    onClicked: {
                        if (hawaiiShellQuickView.location !== Types.LeftEdge)
                            hawaiiShellQuickView.location = Types.LeftEdge;
                    }
                }

                ToolButton {
                    text: qsTr("Top")
                    iconName: "align-vertical-top"
                    exclusiveGroup: positionGroup
                    checkable: true
                    checked: hawaiiShellQuickView.location === Types.TopEdge
                    onClicked: {
                        if (hawaiiShellQuickView.location !== Types.TopEdge)
                            hawaiiShellQuickView.location = Types.TopEdge;
                    }
                }

                ToolButton {
                    text: qsTr("Right")
                    iconName: "align-horizontal-right"
                    exclusiveGroup: positionGroup
                    checkable: true
                    checked: hawaiiShellQuickView.location === Types.RightEdge
                    onClicked: {
                        if (hawaiiShellQuickView.location !== Types.RightEdge)
                            hawaiiShellQuickView.location = Types.RightEdge;
                    }
                }

                ToolButton {
                    text: qsTr("Bottom")
                    iconName: "align-vertical-bottom"
                    exclusiveGroup: positionGroup
                    checkable: true
                    checked: hawaiiShellQuickView.location === Types.BottomEdge
                    onClicked: {
                        if (hawaiiShellQuickView.location !== Types.BottomEdge)
                            hawaiiShellQuickView.location = Types.BottomEdge;
                    }
                }
            }
        }

        GroupBox {
            title: qsTr("Size")

            RowLayout {
                anchors.fill: parent

                ToolButton {
                    text: qsTr("Small")
                    exclusiveGroup: sizeGroup
                    checkable: true
                }

                ToolButton {
                    text: qsTr("Medium")
                    exclusiveGroup: sizeGroup
                    checkable: true
                }

                ToolButton {
                    text: qsTr("Large")
                    exclusiveGroup: sizeGroup
                    checkable: true
                }

                ToolButton {
                    text: qsTr("Huge")
                    exclusiveGroup: sizeGroup
                    checkable: true
                }
            }
        }

        GroupBox {
            title: qsTr("Alignment")

            RowLayout {
                anchors.fill: parent

                ToolButton {
                    text: qsTr("Left")
                    iconName: "align-horizontal-left"
                    exclusiveGroup: alignmentGroup
                    checkable: true
                }

                ToolButton {
                    text: qsTr("Center")
                    iconName: "align-horizontal-center"
                    exclusiveGroup: alignmentGroup
                    checkable: true
                }

                ToolButton {
                    text: qsTr("Right")
                    iconName: "align-horizontal-right"
                    exclusiveGroup: alignmentGroup
                    checkable: true
                }
            }
        }
    }
}
