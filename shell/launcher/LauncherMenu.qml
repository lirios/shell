/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
import Hawaii.Themes 1.0 as Themes
import "../components" as CustomComponents

CustomComponents.PopupMenu {
    id: menu
    width: Themes.Units.dp(200)
    height: column.height
    visualLayer: panel.parent
    visualParent: root
    onShowingChanged: {
        if (showing) {
            mouseArea.hoverEnabled = false;
            tooltip.close();
        } else {
            mouseArea.hoverEnabled = true;
        }
    }

    Column {
        id: column
        width: parent.width

        Repeater {
            model: listView.model.get(root.indexOfThisDelegate).windows

            CustomComponents.MenuItem {
                text: modelData.title
            }
        }
        CustomComponents.MenuSeparator {
            visible: model.hasWindows
        }
        Repeater {
            model: menu.actionList ? menu.actionList : 0

            CustomComponents.MenuItem {
                text: "Action " + index
            }
        }
        CustomComponents.MenuSeparator {
            visible: model.hasActionList
        }
        CustomComponents.MenuItem {
            text: qsTr("New Window")
            visible: model.running
        }
        CustomComponents.MenuSeparator {}
        CustomComponents.MenuItem {
            text: qsTr("Add To Launcher")
            visible: !model.pinned
            onClicked: {
                listView.model.pin(model.appId);
                menu.close();
            }
        }
        CustomComponents.MenuItem {
            text: qsTr("Remove From Launcher")
            visible: model.pinned
            onClicked: {
                listView.model.unpin(model.appId);
                menu.close();
            }
        }
        CustomComponents.MenuSeparator {}
        CustomComponents.MenuItem {
            text: qsTr("Show All Windows")
            visible: model.running
        }
        CustomComponents.MenuItem {
            text: qsTr("Show")
            visible: model.running && !model.active
        }
        CustomComponents.MenuItem {
            text: qsTr("Hide")
            visible: model.running && model.active
        }
        CustomComponents.MenuSeparator {}
        CustomComponents.MenuItem {
            text: qsTr("Quit")
            visible: model.running
            onClicked: {
                if (!listView.model.get(index).quit())
                    console.warn("Failed to quit:", model.appId);
                menu.close();
            }
        }
    }
}
