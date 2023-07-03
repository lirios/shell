// SPDX-FileCopyrightText: 2023 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import Fluid.Controls 1.0 as FluidControls
import Aurora.Client 1.0 as AuroraClient

Window {
    id: splashScreenWindow

    property bool ready: false

    color: "transparent"
    visible: true

    AuroraClient.WlrLayerSurfaceV1 {
        layer: AuroraClient.WlrLayerSurfaceV1.OverlayLayer
        anchors: AuroraClient.WlrLayerSurfaceV1.TopAnchor |
                 AuroraClient.WlrLayerSurfaceV1.BottomAnchor |
                 AuroraClient.WlrLayerSurfaceV1.LeftAnchor |
                 AuroraClient.WlrLayerSurfaceV1.RightAnchor
        keyboardInteractivity: AuroraClient.WlrLayerSurfaceV1.NoKeyboardInteractivity
        exclusiveZone: -1
        role: "splash"
    }

    Timer {
        interval: 250
        running: workspaceReady
        onTriggered: {
            splash.closeWave(splash.width - splash.size, splash.height - splash.size);
            splashScreenWindow.close();
        }
    }

    // Disable mouse pointer
    HoverHandler {
        cursorShape: Qt.BlankCursor
    }

    FluidControls.Wave {
        id: splash

        anchors.fill: parent
        size: 32

        Rectangle {
            anchors.fill: parent
            color: "black"
        }

        Component.onCompleted: {
            openWave(0, 0);
            ready = true;
        }
    }
}
