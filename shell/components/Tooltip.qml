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
import QtQuick.Controls.Material 2.0
import Hawaii.Themes 1.0 as Themes
import Fluid.Controls 1.0 as FluidUi
import "private" as Private

Private.PopupBase {
    property alias text: label.text

    signal closed()

    id: root
    width: label.width + (FluidUi.Units.largeSpacing * 2)
    height: label.height + (FluidUi.Units.smallSpacing * 2)
    onShowingChanged: {
        if (showing)
            showingTimer.start();
        else {
            showingTimer.stop();
            __priv.reallyShowing = false;
        }
    }

    QtObject {
        id: __priv

        property bool reallyShowing: false
    }

    Themes.StyledItem {
        anchors.fill: parent
        style: Themes.Theme.createStyleComponent("Tooltip.qml", root)
        opacity: __priv.reallyShowing ? 1.0 : 0.0
        visible: opacity > 0.0

        Label {
            id: label
            anchors.centerIn: parent
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            style: Text.Raised
            styleColor: Material.secondaryTextColor
            horizontalAlignment: Text.AlignHCenter
            width: Math.min(implicitWidth, FluidUi.Units.dp(100))
            height: implicitHeight
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: FluidUi.Units.longDuration
            }
        }
    }

    Timer {
        id: showingTimer
        interval: 500
        onTriggered: __priv.reallyShowing = true
    }

    Timer {
        id: hidingTimer
        running: showing
        interval: 2500
        onTriggered: root.close()
    }
}
