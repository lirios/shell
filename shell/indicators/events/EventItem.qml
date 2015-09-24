/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Components 1.0 as Components
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes

MouseArea {
    property bool expanded: false

    id: root
    width: parent.width
    height: Themes.Units.gu(2)
    drag.axis: Drag.XAxis
    drag.target: root
    states: [
        State {
            name: "default"
            when: model.hasIcon && (model.body.length > 0)

            AnchorChanges {
                target: titleLabel
                anchors.left: model.hasIcon ? imageItem.right : parent.left
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
            when: !model.hasIcon && (model.body.length === 0)

            AnchorChanges {
                target: titleLabel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        },
        State {
            name: "summaryWithIcons"
            when: model.hasIcon && (model.body.length === 0)

            AnchorChanges {
                target: titleLabel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
            PropertyChanges {
                target: titleLabel
                anchors.leftMargin: imageItem.width + (Themes.Units.smallSpacing * 2)
            }
        }
    ]
    onExpandedChanged: {
        if (expanded && model.body)
            height = Themes.Units.gu(4);
        else
            height = Themes.Units.gu(2);
    }
    onReleased: {
        if (drag.active) {
            if (x > width / 4 || x < width / -4)
                notificationsModel.remove(index);
            else
                x = 0;
        } else if (model.body) {
            expanded = !expanded;
        }
    }

    Behavior on x {
        SpringAnimation {
            spring: 2
            damping: 0.2
        }
    }

    Behavior on height {
        SpringAnimation {
            spring: 5
            damping: 0.3
        }
    }

    Image {
        id: imageItem
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: Themes.Units.smallSpacing
            topMargin: Themes.Units.smallSpacing
        }
        width: Themes.Units.iconSizes.medium
        height: width
        source: width > 0 && height > 0 && model.hasIcon ? "image://notifications/" + model.id : ""
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
        cache: false
        smooth: false
        visible: model.hasIcon
    }

    Controls.Heading {
        id: titleLabel
        level: 4
        font.weight: Font.Bold
        elide: Text.ElideRight
        text: model.summary
        color: Themes.Theme.palette.panel.textColor
        visible: text.length > 0
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Controls.Heading {
        id: bodyLabel
        anchors {
            left: model.hasIcon ? imageItem.right : parent.left
            top: titleLabel.bottom
            right: parent.right
            bottom: parent.bottom
            leftMargin: Themes.Units.smallSpacing * 2
            rightMargin: Themes.Units.smallSpacing * 2
            bottomMargin: Themes.Units.smallSpacing
        }
        level: 5
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        text: model.body
        maximumLineCount: 10
        verticalAlignment: Text.AlignTop
        color: Themes.Theme.palette.panel.textColor
        visible: text.length > 0 && expanded
        onLinkActivated: Qt.openUrlExternally(link)
    }
}
