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
import Qt.labs.controls 1.0
import Fluid.Ui 1.0 as FluidUi
import "../components" as CustomComponents

Menu {
    id: menu
    transformOrigin: Menu.BottomLeft

    Repeater {
        model: listView.model.get(root.indexOfThisDelegate).windows

        MenuItem {
            text: modelData.title
        }
    }

    CustomComponents.MenuSeparator {
        visible: model.hasWindows
    }

    Repeater {
        model: menu.actionList ? menu.actionList : 0

        MenuItem {
            text: "Action " + index
        }
    }
    CustomComponents.MenuSeparator {
        visible: model.hasActionList
    }
    MenuItem {
        text: qsTr("New Window")
        enabled: model.running
    }
    CustomComponents.MenuSeparator {}
    MenuItem {
        text: qsTr("Add To Launcher")
        enabled: !model.pinned
        onClicked: {
            listView.model.pin(model.appId);
            menu.close();
        }
    }
    MenuItem {
        text: qsTr("Remove From Launcher")
        enabled: model.pinned
        onClicked: {
            listView.model.unpin(model.appId);
            menu.close();
        }
    }
    CustomComponents.MenuSeparator {}
    MenuItem {
        text: qsTr("Show All Windows")
        enabled: model.running
    }
    MenuItem {
        text: qsTr("Show")
        enabled: model.running && !model.active
    }
    MenuItem {
        text: qsTr("Hide")
        enabled: model.running && model.active
    }
    CustomComponents.MenuSeparator {}
    MenuItem {
        text: qsTr("Quit")
        enabled: model.running
        onClicked: {
            if (!listView.model.get(index).quit())
                console.warn("Failed to quit:", model.appId);
            menu.close();
        }
    }
}
