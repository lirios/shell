// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Aurora.Compositor 1.0

LayerSurfaceItem {
    property alias stackingLevel: hwLayer.stackingLevel

    WaylandHardwareLayer {
        id: hwLayer
    }
}
