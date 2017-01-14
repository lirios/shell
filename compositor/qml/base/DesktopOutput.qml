/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.5
import GreenIsland 1.0 as GreenIsland

BaseOutput {
    id: output

    property int idleInhibit: 0

    property alias screenView: outputWindow.screenView

    readonly property Item surfacesArea: screenView.surfacesArea

    property alias screenViewComponent: outputWindow.screenViewComponent

    // TODO: this was true for ErrorOutput, false for desktop.Output
    sizeFollowsWindow: false
    automaticFrameCallback: powerState === GreenIsland.ExtendedOutput.PowerStateOn

    window: DesktopOutputWindow {
        id: outputWindow

        output: output
    }

    QtObject {
        id: __private

        property bool idle: false
    }

    function wake() {
        if (!__private.idle)
            return

        console.debug("Power on output", manufacturer, model)
        outputWindow.idleDimmer.fadeOut()
        output.powerState = GreenIsland.ExtendedOutput.PowerStateOn
        __private.idle = false
    }

    function idle() {
        if (__private.idle)
            return;

        console.debug("Standby output", manufacturer, model)
        outputWindow.idleDimmer.fadeIn()
        __private.idle = true
    }
}
