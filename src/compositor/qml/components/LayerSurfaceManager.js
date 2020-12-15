// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

.pragma library
.import Liri.WaylandServer 1.0 as WS

function getParentForLayer(layerSurface, output) {
    switch (layerSurface.layer) {
    case WS.WlrLayerShellV1.BackgroundLayer:
        return output.screenView.desktop.layers.background;
    case WS.WlrLayerShellV1.BottomLayer:
        return output.screenView.desktop.layers.bottom;
    case WS.WlrLayerShellV1.TopLayer:
        return output.screenView.desktop.layers.top;
    case WS.WlrLayerShellV1.OverlayLayer:
        return output.screenView.desktop.layers.overlay;
    default:
        break;
    }

    return undefined;
}
