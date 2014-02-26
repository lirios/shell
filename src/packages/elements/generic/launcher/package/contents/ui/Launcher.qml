/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.2
import QtQuick.Layouts 1.0
import Hawaii.Shell 1.0
import Hawaii.Shell.Core 1.0
import Hawaii.Shell.Settings 1.0
import Hawaii.Shell.Styles 1.0
import Hawaii.Shell.Elements.Launcher 1.0

Element {
    id: launcherContainer
    minimumWidth: listView.orientation == ListView.Horizontal ? listView.count * __priv.tileSize : listView.width
    minimumHeight: listView.orientation == ListView.Horizontal ? listView.height : listView.count * __priv.tileSize
    fillWidth: true
    fillHeight: true

    QtObject {
        id: __priv

        property int iconSize: 48
        property int tileSize: iconSize + (iconSize / 4)
    }

    DropArea {
        anchors.fill: parent
        onDropped: {
            if (drop.proposedAction === Qt.MoveAction || drop.proposedAction === Qt.CopyAction) {
                for (var url in drop.urls) {
                    if (url.substr(0, 4) === "file" && url.substr(-8, 8) === ".desktop")
                        listView.model.model.pinApplication(url);
                    else if (url.substr(0, 4) === "http")
                        listView.model.model.pinUrl(url);
                }

                drop.acceptProposedAction();
            }
        }

        ListView {
            id: listView
            anchors.fill: parent
            clip: true
            orientation: panel.formFactor === Types.Horizontal ? ListView.Horizontal : ListView.Vertical
            model: VisualDataModel {
                model: LauncherModel {}
                delegate: LauncherDelegate {
                    iconSize: __priv.iconSize
                    tileSize: __priv.tileSize
                }
            }
            cacheBuffer: 10000
            interactive: false
            add: Transition {
                NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 250 }
                NumberAnimation { property: "scale"; from: 0.0; to: 1.0; duration: 150 }
            }
            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 250; easing.type: Easing.OutBounce }
            }
        }
    }

    /*
    function enumIconSizeToActualSize(value) {
        switch (value) {
        case LauncherSettings.SmallIconSize:
            return 32;
        case LauncherSettings.MediumIconSize:
            return 48;
        case LauncherSettings.LargeIconSize:
            return 64;
        case LauncherSettings.HugeIconSize:
            return 96;
        default:
            return 48;
        }
    }
    */
}
