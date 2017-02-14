/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini
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

import QtQml 2.2
import QtQuick 2.0
import QtQuick.Controls 2.1
import Liri.Launcher 0.1 as CppLauncher

Menu {
    id: menu
    readonly property var app: launcher.model.get(index)

    transformOrigin: Menu.BottomLeft

    CppLauncher.ProcessRunner {
        id: process
    }

    // Component for separators
    Component {
        id: separatorComponent

        MenuSeparator {}
    }

    Instantiator {
        model: app.desktopFile.actions
        delegate: MenuItem {
            text: model.name
            onTriggered: {
                if (model.command) {
                    process.launchCommand(model.command);
                    menu.close();
                }
            }
        }
        onObjectAdded: menu.insertItem(0, object)
        onObjectRemoved: menu.removeItem(index)
    }

    Instantiator {
        model: app.desktopFile.actions.length > 0 ? 1 : 0
        delegate: MenuSeparator {}
        onObjectAdded: menu.insertItem(app.desktopFile.actions.length, object)
        onObjectRemoved: menu.removeItem(index)
    }

    MenuItem {
        text: qsTr("New Window")
        enabled: model.running
    }

    MenuSeparator {}

    MenuItem {
        text: app.pinned ? qsTr("Unpin from Launcher") : qsTr("Pin to Launcher")
        enabled: model.name
        onTriggered: {
            app.pinned = !app.pinned;
            menu.close();
        }
    }

    MenuSeparator {}

    MenuItem {
        id: quit
        text: qsTr("Quit")
        enabled: app.running
        onTriggered: {
            if (!app.quit())
                console.warn("Failed to quit:", model.appId);
            menu.close();
        }
    }
}
