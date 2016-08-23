/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
import Fluid.Controls 1.0
import org.hawaiios.launcher 0.1 as CppLauncher
import "../components" as CustomComponents

Menu {
    id: menu
    readonly property var app: launcher.model.get(index)

    transformOrigin: Menu.BottomLeft

    CppLauncher.ProcessRunner {
        id: process
    }

    // Component to create application actions
    Component {
        id: actionItemComponent

        MenuItem {
            property string command

            onTriggered: {
                if (command) {
                    process.launchCommand(command);
                    menu.close();
                }
            }
        }
    }

    // Component for separators
    Component {
        id: separatorComponent

        CustomComponents.MenuSeparator {}
    }


    MenuItem {
        text: qsTr("New Window")
        enabled: model.running
    }

    CustomComponents.MenuSeparator {}

    MenuItem {
        text: app.pinned ? qsTr("Unpin from Launcher") : qsTr("Pin to Launcher")
        enabled: model.name
        onTriggered: {
            app.pinned = !app.pinned
            menu.close()
        }
    }

    CustomComponents.MenuSeparator {}

    MenuItem {
        id: ciao
        text: qsTr("Quit")
        enabled: app.running
        onTriggered: {
            if (!app.quit())
                console.warn("Failed to quit:", model.appId);
            menu.close();
        }
    }

    Component.onCompleted: {
        if (launcherItem == undefined)
            return

        var i, item;

        // Add application actions
        for (i = app.actions.length - 1; i >= 0; i--) {
            item = actionItemComponent.createObject(menu.contentItem);
            item.text = app.actions[i].name;
            item.command = app.actions[i].command;
            menu.insertItem(0, item);
        }

        // Add a separator if needed
        if (app.actions.length > 0) {
            item = separatorComponent.createObject(menu.contentItem);
            menu.insertItem(app.actions.length, item);
        }
    }
}
