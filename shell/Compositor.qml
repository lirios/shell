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
import GreenIsland 1.0 as GreenIsland
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaiios.misc 0.1
import "desktop"

GreenIsland.WindowManager {
    readonly property var screens: screenManager.screens
    readonly property var primaryScreen: screenManager.primaryScreen
    //readonly property alias screenView: screenViewLoader.item

    readonly property alias keyBindingsManager: keyBindings
    readonly property alias settings: settings

    signal keyPressed(var event)
    signal keyReleased(var event)

    signal windowSwitchPrev()
    signal windowSwitchNext()
    signal windowSwitchSelect()

    id: hawaiiCompositor
    onCreateSurface: {
        var surface = surfaceComponent.createObject(hawaiiCompositor, {});
        surface.initialize(hawaiiCompositor, client, id, version);
    }

/*

    onKeyReleased: {
        console.log("Key released:", event.key);

        // Window switcher
        if (state == "windowSwitcher") {
            if (event.modifiers === Qt.MetaModifier) {
                // Cycle between windows
                if (event.key === Qt.Key_Tab)
                    compositorRoot.windowSwitchNext();
                else if (event.key === Qt.Key_Backtab)
                    compositorRoot.windowSwitchPrev();
            } else {
                // Give focus to the selected window
                compositorRoot.windowSwitchSelect();

                // Keys released, deactivate switcher
                state = "session";
            }

            event.accepted = true;
            return;
        }

        // Let other components handle this
        event.accepted = false;
    }


    Connections {
        target: GreenIsland.Compositor
        onFadeIn: {
            // Bring user layer up
            compositorRoot.state = "session";
        }
        onFadeOut: {
            // Fade the desktop out
            compositorRoot.state = "splash";
        }
    }
*/

    GreenIsland.KeyBindings {
        id: keyBindings
        onKeyBindingPressed: {
            switch (name) {
                // wm
            case "showDesktop":
                compositorRoot.toggleEffect("RevealDesktop");
                break;
            case "presentWindows":
                compositorRoot.toggleEffect("PresentWindowsGrid");
                break;
            case "switchToWorkspace-1":
                screenView.workspacesView.select(1);
                break;
            case "switchToWorkspace-2":
                screenView.workspacesView.select(2);
                break;
            case "switchToWorkspace-3":
                screenView.workspacesView.select(3);
                break;
            case "switchToWorkspace-4":
                screenView.workspacesView.select(4);
                break;
            case "switchToWorkspace-5":
                screenView.workspacesView.select(5);
                break;
            case "switchToWorkspace-6":
                screenView.workspacesView.select(6);
                break;
            case "switchToWorkspace-7":
                screenView.workspacesView.select(7);
                break;
            case "switchToWorkspace-8":
                screenView.workspacesView.select(8);
                break;
            case "switchToWorkspace-9":
                screenView.workspacesView.select(9);
                break;
            case "switchToWorkspace-10":
                screenView.workspacesView.select(10);
                break;
            case "switchToWorkspace-11":
                screenView.workspacesView.select(11);
                break;
            case "switchToWorkspace-12":
                screenView.workspacesView.select(12);
                break;
            case "switchToWorkspaceLeft":
                screenView.workspacesView.selectPrevious();
                break;
            case "switchToWorkspaceRight":
                screenView.workspacesView.selectNext();
                break;
            case "switchWindows":
            case "switchWindowsBackward":
                if (compositorRoot.state != "windowSwitcher" && surfaceModel.count >= 2)
                    // Activate only when two or more windows are available
                    compositorRoot.state = "windowSwitcher";
                break;
                // sm
            case "abortSession":
                SessionInterface.requestLogOut();
                break;
            case "powerOff":
                SessionInterface.requestPowerOff();
                break;
            case "lockScreen":
                SessionInterface.lockSession();
                break;
            case "activateSession-1":
                SessionInterface.activateSession(1);
                break;
            case "activateSession-2":
                SessionInterface.activateSession(2);
                break;
            case "activateSession-3":
                SessionInterface.activateSession(3);
                break;
            case "activateSession-4":
                SessionInterface.activateSession(4);
                break;
            case "activateSession-5":
                SessionInterface.activateSession(5);
                break;
            case "activateSession-6":
                SessionInterface.activateSession(6);
                break;
            case "activateSession-7":
                SessionInterface.activateSession(7);
                break;
            case "activateSession-8":
                SessionInterface.activateSession(8);
                break;
            case "activateSession-9":
                SessionInterface.activateSession(9);
                break;
            case "activateSession-10":
                SessionInterface.activateSession(10);
                break;
            case "activateSession-11":
                SessionInterface.activateSession(11);
                break;
            case "activateSession-12":
                SessionInterface.activateSession(12);
                break;
            default:
                break;
            }
        }
    }

    /*
     * Components
     */

    // Private properties
    QtObject {
        id: d

        property variant outputs: []
    }

    // Pointer tracking with global coordinates
    GreenIsland.GlobalPointerTracker {
        id: globalPointerTracker
        compositor: hawaiiCompositor
    }

    // Settings
    ShellSettings {
        id: settings
    }

    // Screen manager
    GreenIsland.ScreenManager {
        id: screenManager
        onScreenAdded: {
            var view = outputComponent.createObject(
                        hawaiiCompositor, {
                            "compositor": hawaiiCompositor,
                            "nativeScreen": screen
                        });
            d.outputs.push(view);
        }
        onScreenRemoved: {
            var index = screenManager.indexOf(screen);
            if (index < d.outputs.length) {
                var output = d.outputs[index];
                d.outputs.splice(index, 1);
                output.destroy();
            }
        }
        onPrimaryScreenChanged: {
            var index = screenManager.indexOf(screen);
            if (index < d.outputs.length) {
                hawaiiCompositor.primarySurfacesArea = d.outputs[index].surfacesArea;
                hawaiiCompositor.defaultOutput = d.outputs[index];
            }
        }
    }

    // Surface component
    Component {
        id: surfaceComponent

        GreenIsland.WaylandSurface {}
    }

    // Output component
    Component {
        id: outputComponent

        Output {}
    }

/*
    // Key events filter
    KeyEventFilter {
        id: keyFilter

        Keys.onPressed: compositorRoot.keyPressed(event)
        Keys.onReleased: compositorRoot.keyReleased(event)
    }
    */

    /*
     * Window switcher
     */

/*
    Loader {
        id: windowSwitcherLoader
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        active: false
        asynchronous: true
        source: "WindowSwitcher.qml"
        z: active ? 910 : 899
    }

    // Shield
    Loader {
        id: shieldLoader
        anchors.fill: parent
        asynchronous: true
        z: progress > 0 ? 901 : 899
        opacity: progress

        Behavior on z {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: Themes.Units.longDuration
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: Themes.Units.longDuration
            }
        }
    }
    */

    /*
     * Methods
     */

    function toggleEffect(name) {
        screenView.workspacesView.currentWorkspace.effects.toggle(name);
    }

    function endEffect(name) {
        screenView.workspacesView.currentWorkspace.effects.end(name);
    }

    function addWindowToEffect(window) {
        screenView.workspacesView.currentWorkspace.effects.addWindow(window);
    }

    function removeWindowFromEffect(window) {
        screenView.workspacesView.currentWorkspace.effects.removeWindow(window);
    }

    function moveFront(window) {
        return WindowManagement.moveFront(window);
    }

    function enableInput() {
        var i;
        for (i = 0; i < compositorRoot.surfaceModel.count; i++) {
            var window = compositorRoot.surfaceModel.get(i).item;
            window.child.focus = true;
        }
    }

    function disableInput() {
        var i;
        for (i = 0; i < compositorRoot.surfaceModel.count; i++) {
            var window = compositorRoot.surfaceModel.get(i).window;
            window.deactivate();
        }
        GreenIsland.Compositor.clearKeyboardFocus();
    }
}
