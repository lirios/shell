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
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import "private" as Private

MouseArea {
    property ListModel model
    property Item visualParent: parent
    readonly property bool showing: popupLoader.loaded ? popupLoader.item.showing : false
    property list<Item> content

    signal triggered()
    signal closed()

    id: root
    acceptedButtons: Qt.LeftButton
    hoverEnabled: true
    preventStealing: true
    width: popupLoader.loaded ? popupLoader.item.width : 0
    height: popupLoader.loaded ? popupLoader.item.height : 0
    onClicked: root.closed()

    Component {
        id: popupComponent

        Private.BasicPopupMenu {
            id: popup
            content: root.content
            visualParent: root.visualParent
            visualLayer: notificationsLayer
            onShowingChanged: {
                if (showing)
                    root.triggered();
            }
            onClosed: {
                popupLoader.sourceComponent = undefined;
                root.closed();
            }

            /*
            Components.MouseGrabber {
                id: grabber
                acceptedButtons: Qt.AllButtons
                anchors.fill: parent
                onClickedOutside: root.close()
            }
            */
        }
    }

    Loader {
        property bool loaded: status == Loader.Ready

        id: popupLoader
    }

    function open() {
        if (!popupLoader.loaded)
            popupLoader.sourceComponent = popupComponent;
        popupLoader.item.open();
    }

    function openAt(x, y) {
        if (!popupLoader.loaded)
            popupLoader.sourceComponent = popupComponent;
        popupLoader.item.openAt(x, y);
    }

    function close() {
        if (popupLoader.loaded)
            popupLoader.item.close();
    }
}

