// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Aurora.Compositor 1.0
import Liri.private.shell 1.0 as P
import Liri.Session 1.0 as Session

P.WaylandOutput {
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

    sizeFollowsWindow: false
    automaticFrameCallback: {
        if (screen)
            return screen.enabled && screen.powerState === P.ScreenItem.PowerStateOn;
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
        screen.powerState = P.ScreenItem.PowerStateOn;
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
