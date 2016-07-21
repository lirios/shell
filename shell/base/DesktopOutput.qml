/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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

import QtQuick 2.5
import GreenIsland 1.0 as GreenIsland

BaseOutput {
    id: output

    property int idleInhibit: 0

    property alias screenView: outputWindow.screenView
    property alias idleDimmer: outputWindow.idleDimmer

    property alias screenViewComponent: outputWindow.screenViewComponent
    property alias idleDimmerComponent: outputWindow.idleDimmerComponent

    // TODO: this was true for ErrorOutput, false for desktop.Output
    sizeFollowsWindow: false
    automaticFrameCallback: powerState === GreenIsland.ExtendedOutput.PowerStateOn

    onPowerStateChanged: {
        // Show the screen when the power goes back
        if (output.powerState === GreenIsland.ExtendedOutput.PowerStateOn)
            idleDimmer.fadeOut()
    }

    window: DesktopOutputWindow {
        id: outputWindow

        output: output
    }

    function wake() {
        idleDimmer.fadeOut()
        output.powerState = GreenIsland.ExtendedOutput.PowerStateOn
    }

    function idle() {
        idleDimmer.fadeIn()
    }
}
