/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2012-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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
import GreenIsland 1.0 as GreenIsland
import Liri.Launcher 0.1
import Liri.Shell 1.0
import Vibe.PulseAudio 1.0
import "base"
import "desktop"

WindowCompositor {
    id: compositor

    property int idleInhibit: 0

    readonly property alias settings: settings

    onOpenUrl: processRunner.launchCommand("xdg-open %1".arg(url))

    screenViewComponent: ScreenView {}

    function wake() {
        var i;
        for (i = 0; i < __private.outputs.length; i++) {
            idleTimer.restart();
            __private.outputs[i].wake();
        }

        SessionInterface.idle = false;
    }

    function idle() {
        var i;
        for (i = 0; i < __private.outputs.length; i++)
            __private.outputs[i].idle();

        SessionInterface.idle = true;
    }

    function activateWindows(appId) {
        // Activate all windows of this application and unminimize
        for (var i = 0; i < compositor.windowsModel.count; i++) {
            var window = compositor.windowsModel.get(i).window

            if (window.appId === appId) {
                window.minimized = false
                window.activate()
            }
        }
    }

    ApplicationManager {
        id: applicationManager

        applicationManager: compositor.applicationManager
    }

    LauncherModel {
        id: launcherModel
        applicationManager: applicationManager
    }

    AppsModel {
        id: appsModel
        applicationManager: applicationManager
    }

    DateTime {
        id: dateTime
    }

    ShellSettings {
        id: settings
    }

    ProcessRunner {
        id: processRunner
    }

    KeyBindings {}

    VolumeControl {
        id: volumeControl
    }

    Timer {
        id: idleTimer
        interval: settings.session.idleDelay * 1000
        running: true
        repeat: true
        onTriggered: {
            var i, output, idleHint = false;
            for (i = 0; i < __private.outputs.length; i++) {
                output = __private.outputs[i];
                if (idleInhibit + output.idleInhibit == 0) {
                    output.idle();
                    idleHint = true;
                }
            }

            SessionInterface.idle = idleHint;
        }
    }
}
