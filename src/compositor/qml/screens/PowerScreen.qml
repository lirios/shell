// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls.Material 2.15

Item {
    id: powerScreen

    property bool active

    anchors.fill: parent

    signal canceled()

    onActiveChanged: {
        if (!output.primary)
            return

        if (active)
            powerDialog.open()
        else
            powerDialog.close()
    }

    Rectangle {
        anchors.fill: parent

        opacity: powerScreen.active && !output.primary ? 1 : 0
        color: Material.backgroundDimColor

        Behavior on opacity { NumberAnimation { duration: 150 } }
    }

    PowerDialog {
        id: powerDialog

        onCanceled: powerScreen.canceled()
    }
}
