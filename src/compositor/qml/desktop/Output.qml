// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import Aurora.Compositor
import Aurora.Platform
import Liri.Session as Session

WaylandOutput {
    id: output

    readonly property bool primary: liriCompositor.defaultOutput === this

    property var viewsBySurface: ({})

    readonly property alias lockSurfacesModel: outputWindow.lockSurfacesModel
    readonly property alias layerSurfacesModel: outputWindow.layerSurfacesModel
    readonly property alias currentWorkspace: outputWindow.currentWorkspace

    property alias showFps: outputWindow.showFps
    property alias showInformation: outputWindow.showInformation

    property var exportDmabufFrame: null

    property bool __idle: false

    automaticFrameCallback: {
        if (platformOutput)
            return platformOutput.enabled && platformOutput.powerState === PlatformOutput.PowerState.On;
        return true;
    }

    window: OutputWindow {
        id: outputWindow
    }

    /*
     * Methods
     */

    function reveal() {
        outputWindow.reveal();
    }

    function wake() {
        if (!__idle)
            return;

        console.debug("Power on output", manufacturer, model);
        idleDimmer.fadeOut();
        platformOutput.powerState = PlatformOutput.PowerState.On;
        __idle = false;
    }

    function idle() {
        if (__idle)
            return;

        console.debug("Standby output", manufacturer, model);
        idleDimmer.fadeIn();
        __idle = true;
    }

    function flash() {
        outputWindow.flash();
    }

    function selectPreviousWorkspace() {
        outputWindow.selectPrevious();
    }

    function selectNextWorkspace() {
        outputWindow.selectNext();
    }

    function selectWorkspace(num) {
        outputWindow.select(num);
    }
}
