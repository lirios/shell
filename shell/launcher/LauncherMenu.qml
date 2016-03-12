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
import Qt.labs.controls 1.0 as LabsControls
import Fluid.Ui 1.0 as FluidUi
import "../components" as CustomComponents

LabsControls.Menu {
    id: menu
    transformOrigin: LabsControls.Menu.TopLeft

    Repeater {
        model: listView.model.get(root.indexOfThisDelegate).windows

        LabsControls.MenuItem {
            text: modelData.title
        }
    }

    CustomComponents.MenuSeparator {
        visible: model.hasWindows
    }

    Repeater {
        model: menu.actionList ? menu.actionList : 0

        LabsControls.MenuItem {
            text: "Action " + index
        }
    }
    CustomComponents.MenuSeparator {
        visible: model.hasActionList
    }
    LabsControls.MenuItem {
        text: qsTr("New Window")
        visible: model.running
    }
    CustomComponents.MenuSeparator {}
    LabsControls.MenuItem {
        text: qsTr("Add To Launcher")
        visible: !model.pinned
        onClicked: {
            listView.model.pin(model.appId);
            menu.close();
        }
    }
    LabsControls.MenuItem {
        text: qsTr("Remove From Launcher")
        visible: model.pinned
        onClicked: {
            listView.model.unpin(model.appId);
            menu.close();
        }
    }
    CustomComponents.MenuSeparator {}
    LabsControls.MenuItem {
        text: qsTr("Show All Windows")
        visible: model.running
    }
    LabsControls.MenuItem {
        text: qsTr("Show")
        visible: model.running && !model.active
    }
    LabsControls.MenuItem {
        text: qsTr("Hide")
        visible: model.running && model.active
    }
    CustomComponents.MenuSeparator {}
    LabsControls.MenuItem {
        text: qsTr("Quit")
        visible: model.running
        onClicked: {
            if (!listView.model.get(index).quit())
                console.warn("Failed to quit:", model.appId);
            menu.close();
        }
    }
}
