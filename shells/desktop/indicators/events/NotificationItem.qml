/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Components 1.0 as Components
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes
import org.kde.kquickcontrolsaddons 2.0 as KQuickControls

Item {
    property alias icon: appIconItem.iconName
    property alias image: imageItem.image
    property bool hasIcon: false
    property bool hasImage: false
    property alias summary: titleLabel.text
    property alias body: bodyLabel.text
    property ListModel actions: ListModel {}

    signal actionInvoked(string actionId)

    id: root
    width: parent.width
    height: implicitHeight
    implicitHeight: {
        // Return maximum height possible, at least 5 grid units
        var minHeight = actionsColumn.height + (Themes.Units.smallSpacing * 4);
        var maxHeight = Math.max(appIconItem.height, titleLabel.paintedHeight + bodyLabel.implicitHeight) + (Themes.Units.smallSpacing * 4);
        return Math.max(minHeight, Math.min(maxHeight, Themes.Units.gu(5)));
    }
    states: [
        State {
            name: "default"
            when: (appIconItem.visible || imageItem.visible) && (bodyLabel.visible || actionsColumn.visible)

            AnchorChanges {
                target: titleLabel
                anchors.left: hasIcon || hasImage ? appIconItem.right : parent.left
                anchors.top: parent.top
                anchors.right: parent.right
            }
            PropertyChanges {
                target: titleLabel
                anchors.leftMargin: Themes.Units.smallSpacing * 2
                anchors.topMargin: Themes.Units.smallSpacing
            }
        },
        State {
            name: "summaryOnly"
            when: !appIconItem.visible && !imageItem.visible && !bodyLabel.visible && !actionsColumn.visible

            AnchorChanges {
                target: titleLabel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        },
        State {
            name: "summaryWithIcons"
            when: (appIconItem.visible || imageItem.visible) && !bodyLabel.visible && !actionsColumn.visible

            AnchorChanges {
                target: titleLabel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
            PropertyChanges {
                target: titleLabel
                anchors.leftMargin: appIconItem.width + (Themes.Units.smallSpacing * 2)
            }
        }
    ]

    Components.Icon {
        id: appIconItem
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: Themes.Units.smallSpacing
            topMargin: Themes.Units.smallSpacing
        }
        width: Themes.Units.iconSizes.large
        height: width
        color: Themes.Theme.palette.panel.textColor
        visible: hasIcon
    }

    KQuickControls.QImageItem {
        id: imageItem
        anchors.fill: appIconItem
        image: undefined
        fillMode: Image.PreserveAspectFit
        visible: hasImage
    }

    Controls.Heading {
        id: titleLabel
        level: 4
        font.weight: Font.Bold
        elide: Text.ElideRight
        color: Themes.Theme.palette.panel.textColor
        visible: text.length > 0
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Controls.Heading {
        id: bodyLabel
        anchors {
            left: hasIcon || hasImage ? appIconItem.right : parent.left
            top: titleLabel.bottom
            right: actionsColumn.visible ? actionsColumn.left : parent.right
            bottom: parent.bottom
            leftMargin: Themes.Units.smallSpacing * 2
            rightMargin: Themes.Units.smallSpacing * 2
            bottomMargin: Themes.Units.smallSpacing
        }
        level: 5
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        maximumLineCount: 10
        verticalAlignment: Text.AlignTop
        color: Themes.Theme.palette.panel.textColor
        visible: text.length > 0
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Column {
        id: actionsColumn
        anchors {
            top: titleLabel.bottom
            right: parent.right
            topMargin: Themes.Units.smallSpacing
        }
        spacing: Themes.Units.smallSpacing
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
