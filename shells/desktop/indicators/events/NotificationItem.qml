/****************************************************************************
 * This file is part of Hawaii Shell.
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
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0 as KQuickControls


Item {
    property alias icon: appIconItem.icon
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
        var minHeight = actionsColumn.height + (units.smallSpacing * 4);
        var maxHeight = Math.max(appIconItem.height, titleLabel.paintedHeight + bodyLabel.implicitHeight) + (units.smallSpacing * 4);
        return Math.max(minHeight, Math.min(maxHeight, units.gridUnit * 5));
    }

    KQuickControls.QIconItem {
        id: appIconItem
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: units.smallSpacing
            topMargin: units.smallSpacing
        }
        width: units.iconSizes.large
        height: width
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
        anchors {
            left: hasIcon || hasImage ? appIconItem.right : parent.left
            top: parent.top
            right: parent.right
            leftMargin: units.smallSpacing * 2
        }
        level: 4
        font.weight: Font.Bold
        elide: Text.ElideRight
        color: Theme.palette.panel.textColor
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
            leftMargin: units.smallSpacing * 2
            rightMargin: units.smallSpacing * 2
            bottomMargin: units.smallSpacing
        }
        level: 5
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        maximumLineCount: 10
        verticalAlignment: Text.AlignTop
        color: Theme.palette.panel.textColor
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Column {
        id: actionsColumn
        anchors {
            top: titleLabel.bottom
            right: parent.right
            topMargin: units.smallSpacing
        }
        spacing: units.smallSpacing
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
