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
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes
import "private" as Private

Private.PopupBase {
    default property alias content: container.data

    id: root
    width: implicitWidth
    height: implicitHeight
    implicitWidth: Themes.Units.dp(300)
    onShowingChanged: {
        if (showing)
            showAnimation.start();
        else
            hideAnimation.start();
    }

    Themes.StyledItem {
        id: container
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        style: Themes.Theme.createStyleComponent("PopupMenu.qml", container)
        height: parent.height
        visible: false

        Controls.PopupBehavior {
            anchors.fill: parent
            onDismissed: root.close()
        }
    }

    /*
     * Animations
     */

    SequentialAnimation {
        id: showAnimation

        PropertyAction {
            target: container
            property: "visible"
            value: true
        }

        ParallelAnimation {
            NumberAnimation {
                target: root
                property: "opacity"
                easing.type: Easing.OutQuad
                from: 0.0
                to: 1.0
                duration: 150
            }
            NumberAnimation {
                target: root
                property: "scale"
                easing.type: Easing.OutQuad
                from: 0.9
                to: 1.0
                duration: 150
            }
        }
    }

    SequentialAnimation {
        id: hideAnimation

        ParallelAnimation {
            NumberAnimation {
                target: root
                property: "scale"
                easing.type: Easing.OutQuad
                from: 1.0
                to: 0.8
                duration: 150
            }
            NumberAnimation {
                target: root
                property: "opacity"
                easing.type: Easing.OutQuad
                from: 1.0
                to: 0.0
                duration: 250
            }
        }

        PropertyAction {
            target: container
            property: "visible"
            value: false
        }
    }
}

