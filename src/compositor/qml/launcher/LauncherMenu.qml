// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQml 2.2
import QtQuick 2.15
import QtQuick.Controls 2.15

Menu {
    id: menu

    property var actions: model.application.actions

    transformOrigin: Menu.BottomLeft

    Instantiator {
        id: actionsInstantiator
        model: actions
        delegate: MenuItem {
            text: model.name
            onTriggered: {
                if (model.command) {
                    SessionInterface.launchCommand(model.command);
                    menu.close();
                }
            }
        }
        onObjectAdded: menu.insertItem(0, object)
        onObjectRemoved: menu.removeItem(index)
    }

    Instantiator {
        model: actionsInstantiator.count > 0 ? 1 : 0
        delegate: MenuSeparator {}
        onObjectAdded: menu.insertItem(actionsInstantiator.count, object)
        onObjectRemoved: menu.removeItem(index)
    }

    MenuItem {
        text: qsTr("New Window")
        enabled: model.running
    }

    MenuSeparator {}

    MenuItem {
        text: model.pinned ? qsTr("Unpin from Launcher") : qsTr("Pin to Launcher")
        enabled: model.name
        onTriggered: {
            model.application.pinned = !model.application.pinned;
            menu.close();
        }
    }

    MenuSeparator {}

    MenuItem {
        id: quit
        text: qsTr("Quit")
        enabled: model.running
        onTriggered: {
            if (!model.application.quit())
                console.warn("Failed to quit:", model.appId);
            menu.close();
        }
    }
}
