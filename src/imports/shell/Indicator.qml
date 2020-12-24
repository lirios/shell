// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Fluid.Controls 1.0 as FluidControls

Item {
    property string title
    property alias text: label.text
    property alias iconName: icon.name
    property alias iconSource: icon.source

    readonly property int iconSize: height >= 40 ? 56 * 0.36 : height * 0.45

    width: visible ? text ? label.paintedWidth + 16 : 32 : 0
    height: parent.height

    FluidControls.Icon {
        id: icon

        size: iconSize
        anchors.centerIn: parent
    }

    Label {
        id: label

        anchors.centerIn: parent
        font.pixelSize: 15
    }
}
