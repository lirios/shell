// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Fluid.Controls as FluidControls

Rectangle {
    color: "black"

    TapHandler {
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        cursorShape: Qt.ArrowCursor
    }

    HoverHandler {
        cursorShape: Qt.ArrowCursor
    }

    ColumnLayout {
        anchors.centerIn: parent

        FluidControls.Placeholder {
            Layout.alignment: Qt.AlignVCenter

            icon.source: FluidControls.Utils.iconUrl("action/lock")
            text: qsTr("Session Locked")
            subText: qsTr("The session has been locked")
        }

        Button {
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

            text: qsTr("Log Out")

            onClicked: liriCompositor.quit()
        }
    }
}
