/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

import QtQuick 2.15
import QtQuick.Controls 2.15
import Fluid.Controls 1.0
import Liri.Notifications 1.0

Item {
    property alias icon: imageItem.source
    property bool hasIcon: false
    property alias summary: titleLabel.text
    property alias body: bodyLabel.text
    property ListModel actions: ListModel {}

    signal actionInvoked(string actionId)

    id: root
    height: implicitHeight
    implicitHeight: {
        // Return maximum height possible, at least 5 grid units
        var minHeight = actionsColumn.height + (Units.smallSpacing * 4);
        var maxHeight = Math.max(imageItem.height, titleLabel.paintedHeight + bodyLabel.implicitHeight) + (Units.smallSpacing * 4);
        return Math.max(minHeight, Math.min(maxHeight, Units.gu(5)));
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
                anchors.leftMargin: Units.smallSpacing * 2
                anchors.topMargin: Units.smallSpacing
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
                anchors.leftMargin: imageItem.width + (Units.smallSpacing * 2)
            }
        }
    ]

    Image {
        id: imageItem
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: Units.smallSpacing
            topMargin: Units.smallSpacing
        }
        width: Units.iconSizes.large
        height: width
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
        cache: false
        smooth: false
        visible: hasIcon
    }

    SubheadingLabel {
        id: titleLabel
        font.weight: Font.Bold
        elide: Text.ElideRight
        visible: text.length > 0
        onLinkActivated: Qt.openUrlExternally(link)
    }

    BodyLabel {
        id: bodyLabel
        anchors {
            left: hasIcon ? imageItem.right : parent.left
            top: titleLabel.bottom
            right: actionsColumn.visible ? actionsColumn.left : parent.right
            bottom: parent.bottom
            leftMargin: Units.smallSpacing * 2
            rightMargin: Units.smallSpacing * 2
            bottomMargin: Units.smallSpacing
        }
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
            topMargin: Units.smallSpacing
        }
        spacing: Units.smallSpacing
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
