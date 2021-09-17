// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls.Material 2.15

Rectangle {
    id: modalOverlay

    Material.theme: Material.Light

    anchors.fill: parent
    color: Material.backgroundDimColor
    opacity: liriCompositor.showModalOverlay ? 1.0 : 0.0
    enabled: liriCompositor.showModalOverlay

    Behavior on opacity {
        NumberAnimation { duration: 150 }
    }

    HoverHandler {
        grabPermissions: HoverHandler.CanTakeOverFromAnything
    }

    TapHandler {
        grabPermissions: TapHandler.CanTakeOverFromAnything
        onTapped: {
            liriModal.sendDone();
        }
    }
}
