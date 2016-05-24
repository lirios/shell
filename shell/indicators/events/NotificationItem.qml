/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 2.0
import Hawaii.Controls 1.0 as Controls
import Fluid.Ui 1.0 as FluidUi

Item {
    property alias icon: imageItem.source
    property bool hasIcon: false
    property alias summary: titleLabel.text
    property alias body: bodyLabel.text
    property ListModel actions: ListModel {}

    signal actionInvoked(string actionId)

    id: root
    width: parent.width
    height: implicitHeight
    implicitHeight: {
        // Return maximum height possible, at least 5 grid units
        var minHeight = actionsColumn.height + (FluidUi.Units.smallSpacing * 4);
        var maxHeight = Math.max(imageItem.height, titleLabel.paintedHeight + bodyLabel.implicitHeight) + (FluidUi.Units.smallSpacing * 4);
        return Math.max(minHeight, Math.min(maxHeight, FluidUi.Units.gu(5)));
    }
    states: [
        State {
            name: "default"
            when: hasIcon && (bodyLabel.visible || actionsColumn.visible)

            AnchorChanges {
                target: titleLabel
                anchors.left: hasIcon ? imageItem.right : parent.left
                anchors.top: parent.top
                anchors.right: parent.right
            }
            PropertyChanges {
                target: titleLabel
                anchors.leftMargin: FluidUi.Units.smallSpacing * 2
                anchors.topMargin: FluidUi.Units.smallSpacing
            }
        },
        State {
            name: "summaryOnly"
            when: !hasIcon && !bodyLabel.visible && !actionsColumn.visible

            AnchorChanges {
                target: titleLabel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        },
        State {
            name: "summaryWithIcons"
            when: hasIcon && !bodyLabel.visible && !actionsColumn.visible

            AnchorChanges {
                target: titleLabel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
            PropertyChanges {
                target: titleLabel
                anchors.leftMargin: imageItem.width + (FluidUi.Units.smallSpacing * 2)
            }
        }
    ]

    Image {
        id: imageItem
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: FluidUi.Units.smallSpacing
            topMargin: FluidUi.Units.smallSpacing
        }
        width: FluidUi.Units.iconSizes.large
        height: width
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
        cache: false
        smooth: false
        visible: hasIcon
    }

    Controls.Heading {
        id: titleLabel
        level: 4
        font.weight: Font.Bold
        elide: Text.ElideRight
        visible: text.length > 0
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Controls.Heading {
        id: bodyLabel
        anchors {
            left: hasIcon ? imageItem.right : parent.left
            top: titleLabel.bottom
            right: actionsColumn.visible ? actionsColumn.left : parent.right
            bottom: parent.bottom
            leftMargin: FluidUi.Units.smallSpacing * 2
            rightMargin: FluidUi.Units.smallSpacing * 2
            bottomMargin: FluidUi.Units.smallSpacing
        }
        level: 5
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        maximumLineCount: 10
        verticalAlignment: Text.AlignTop
        visible: text.length > 0
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Column {
        id: actionsColumn
        anchors {
            top: titleLabel.bottom
            right: parent.right
            topMargin: FluidUi.Units.smallSpacing
        }
        spacing: FluidUi.Units.smallSpacing
        height: childrenRect.height
        visible: actions.count > 0

        Repeater {
            id: actionsRepeater
            model: actions

            Button {
                text: model.text
                onClicked: root.actionInvoked(model.id)
            }
        }
    }
}
