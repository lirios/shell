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
import QtCompositor 1.0
import GreenIsland 1.0 as GreenIsland
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaii.misc 0.1
import org.hawaii.session 1.0 as Session
import "desktop"
import "windows"
import "windows/WindowManagement.js" as WindowManagement

Item {
    readonly property alias screenView: screenViewLoader.item
    readonly property alias surfaceModel: surfaceModel
    property var activeWindow: null
    readonly property int activeWindowIndex: WindowManagement.getActiveWindowIndex()
    readonly property var windowList: WindowManagement.windowList

    readonly property alias topLevelWindowComponent: topLevelWindowComponent
    readonly property alias popupWindowComponent: popupWindowComponent
    readonly property alias transientWindowComponent: transientWindowComponent

    signal keyPressed(var event)
    signal keyReleased(var event)

    signal windowSwitchPrev()
    signal windowSwitchNext()
    signal windowSwitchSelect()

    id: compositorRoot
    state: "splash"
    states: [
        State {
            name: "splash"
            PropertyChanges { target: splashScreen; opacity: 1.0 }
            PropertyChanges { target: keyFilter; enabled: false }
            StateChangeScript { script: disableInput() }
        },
        State {
            name: "session"
            PropertyChanges { target: keyFilter; enabled: true }
            PropertyChanges { target: splashScreen; opacity: 0.0 }
            PropertyChanges { target: windowSwitcherLoader; active: false }
            PropertyChanges { target: shieldLoader; source: ""; visible: false }
            PropertyChanges { target: logoutLoader; loadComponent: false }
            PropertyChanges { target: lockScreenLoader; loadComponent: false }
            PropertyChanges { target: splashScreen; opacity: 0.0 }
            StateChangeScript { script: enableInput() }
        },
        State {
            name: "windowSwitcher"
            PropertyChanges { target: windowSwitcherLoader; active: true }
            StateChangeScript { script: disableInput() }
        },
        State {
            name: "logout"
            PropertyChanges { target: keyFilter; enabled: false }
            //PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
            PropertyChanges { target: logoutLoader; loadComponent: true; mode: "logout" }
            StateChangeScript { script: disableInput() }
        },
        State {
            name: "poweroff"
            PropertyChanges { target: keyFilter; enabled: false }
            //PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
            PropertyChanges { target: logoutLoader; loadComponent: true; mode: "poweroff" }
            StateChangeScript { script: disableInput() }
        },
        State {
            name: "restart"
            PropertyChanges { target: keyFilter; enabled: false }
            //PropertyChanges { target: shieldLoader; source: "Shield.qml"; z: 909 }
            PropertyChanges { target: logoutLoader; loadComponent: true; mode: "restart" }
            StateChangeScript { script: disableInput() }
        },
        State {
            name: "lock"
            PropertyChanges { target: keyFilter; enabled: false }
            PropertyChanges { target: windowSwitcherLoader; active: false }
            PropertyChanges { target: shieldLoader; source: ""; visible: false }
            PropertyChanges { target: logoutLoader; loadComponent: false }
            PropertyChanges { target: lockScreenLoader; loadComponent: true }
            StateChangeScript { script: disableInput() }
        },
        State {
            name: "shield"
            PropertyChanges { target: shieldLoader; source: "Shield.qml"; visible: true }
        }
    ]
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

    ListModel {
        id: surfaceModel
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
        onWindowMapped: {
            // A window was mapped
            WindowManagement.windowMapped(window);
        }
        onWindowUnmapped: {
            // A window was unmapped
            WindowManagement.windowUnmapped(window);
        }
        onWindowDestroyed: {
            // A window was unmapped
            WindowManagement.windowDestroyed(id);
        }
    }

    Connections {
        target: GreenIsland.KeyBindings
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
                Session.SessionInterface.requestLogOut();
                break;
            case "powerOff":
                Session.SessionInterface.requestPowerOff();
                break;
            case "lockScreen":
                Session.SessionInterface.lockSession();
                break;
            case "activateSession-1":
                Session.SessionInterface.activateSession(1);
                break;
            case "activateSession-2":
                Session.SessionInterface.activateSession(2);
                break;
            case "activateSession-3":
                Session.SessionInterface.activateSession(3);
                break;
            case "activateSession-4":
                Session.SessionInterface.activateSession(4);
                break;
            case "activateSession-5":
                Session.SessionInterface.activateSession(5);
                break;
            case "activateSession-6":
                Session.SessionInterface.activateSession(6);
                break;
            case "activateSession-7":
                Session.SessionInterface.activateSession(7);
                break;
            case "activateSession-8":
                Session.SessionInterface.activateSession(8);
                break;
            case "activateSession-9":
                Session.SessionInterface.activateSession(9);
                break;
            case "activateSession-10":
                Session.SessionInterface.activateSession(10);
                break;
            case "activateSession-11":
                Session.SessionInterface.activateSession(11);
                break;
            case "activateSession-12":
                Session.SessionInterface.activateSession(12);
                break;
            default:
                break;
            }
        }
    }

    Connections {
        target: Session.SessionInterface
        onSessionLocked: compositorRoot.state = "lock"
        onSessionUnlocked: compositorRoot.state = "session"
        onShutdownRequestCanceled: compositorRoot.state = "session"
        onLogOutRequested: compositorRoot.state = "logout"
        onPowerOffRequested: compositorRoot.state = "poweroff"
        onRestartRequested: compositorRoot.state = "restart"
    }

    /*
     * Components
     */

    // FPS counter
    Text {
        anchors {
            top: parent.top
            right: parent.right
        }
        z: 1000
        text: fpsCounter.fps
        font.pointSize: 36
        style: Text.Raised
        styleColor: "#222"
        color: "white"
        visible: false

        GreenIsland.FpsCounter {
            id: fpsCounter
        }
    }

    // Key events filter
    KeyEventFilter {
        id: keyFilter

        Keys.onPressed: compositorRoot.keyPressed(event)
        Keys.onReleased: compositorRoot.keyReleased(event)
    }

    // Top level window component
    Component {
        id: topLevelWindowComponent

        TopLevelWindow {}
    }

    // Popup window component
    Component {
        id: popupWindowComponent

        PopupWindow {}
    }

    // Transient window component
    Component {
        id: transientWindowComponent

        TransientWindow {}
    }

    /*
     * Splash
     */

    Loader {
        id: splashScreen
        anchors.fill: parent
        opacity: 0.0
        onOpacityChanged: {
            if (opacity == 1.0) {
                splashScreen.z = 910;
                splashScreen.source = "SplashScreen.qml";
            } else if (opacity == 0.0) {
                splashScreenTimer.start();
            }
        }

        // Unload after a while so that the opacity animation is visible
        Timer {
            id: splashScreenTimer
            running: false
            interval: 5000
            onTriggered: {
                splashScreen.z = 899;
                splashScreen.source = "";
            }
        }

        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InSine
                duration: Themes.Units.longDuration
            }
        }
    }

    /*
     * Screen view
     */

    Loader {
        property bool alreadyLoaded: false

        id: screenViewLoader
        anchors.fill: parent
        asynchronous: true
        source: "desktop/ScreenView.qml"
        z: 900
        onLoaded: {
            // We asynchronously load the screen component when the splash state
            // is reached. As soon as the component is loaded we switch to
            // the session state
            compositorRoot.state = "session";
        }
        onItemChanged: {
            if (!item || alreadyLoaded)
                return;

            // Create default 4 workspaces
            var i;
            for (i = 0; i < 4; i++)
                item.workspacesView.add();
            item.workspacesView.select(0);

            // Setup workspaces only once
            alreadyLoaded = true;
        }
        onStatusChanged: {
            // Show an error screen instead
            if (status == Loader.Error)
                source = "ErrorScreen.qml";
        }
    }

    /*
     * Window switcher
     */

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

    /*
     * Logout screen
     */

    Components.Loadable {
        property bool loadComponent: false
        property string mode: "logout"

        id: logoutLoader
        anchors.fill: parent
        asynchronous: true
        component: Component {
            LogoutScreen {
                mode: logoutLoader.mode
            }
        }
        z: 910
        onLoadComponentChanged: if (loadComponent) show(); else hide();
    }

    Connections {
        target: logoutLoader.item
        onSuspendRequested: compositorRoot.state = "lock"
        onCancel: Session.SessionInterface.cancelShutdownRequest()
    }

    /*
     * Lock screen
     */

    Component {
        id: primaryLockScreenComponent

        LockScreen {}
    }

    Component {
        id: secondaryLockScreenComponent

        SecondaryLockScreen {}
    }

    Components.Loadable {
        property bool loadComponent: false

        id: lockScreenLoader
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        asynchronous: true
        component: _greenisland_output.primary ? primaryLockScreenComponent : secondaryLockScreenComponent
        z: 910
        onLoadComponentChanged: if (loadComponent) show(); else hide();
    }

    /*
     * Component
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
