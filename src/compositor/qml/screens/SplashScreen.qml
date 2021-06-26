// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Rectangle {
    // Same color as Plymouth and SDDM
    color: Material.color(Material.BlueGrey, Material.Shade800)

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.Blue

    BusyIndicator {
        anchors.centerIn: parent
    }
}
